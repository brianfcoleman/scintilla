#include <DocumentManager.h>

#include <MessageSender.h>
#include <ScopeExit.h>

#include <ILexer.h>

#include <QFile>
#include <QIODevice>
#include <QObject>
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>

#include <algorithm>

namespace cursor {
bool DocumentManager::loadDocument(const QString& filePath) {
    const int initialDocumentSizeBytes = 1024;
    ILoader* loader = nullptr;
    ScopeExit releaseLoader([loader]() {
        if (loader == nullptr) {
            return;
        }
        loader->Release();
    });
    loader = messageSender().createLoader(initialDocumentSizeBytes);
    if (!loader) {
        qDebug() << "Failed to create loader for document " << filePath;
        return false;
    }
    qDebug() << "Created loader for document " << filePath;
    auto future = QtConcurrent::run([filePath, loader] () -> Document {
        using namespace cursor;
        auto documentLoader = loader;
        ScopeExit releaseLoader([documentLoader]() {
            if (documentLoader == nullptr) {
                return;
            }
            documentLoader->Release();
        });
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open document " << filePath;
            return Document();
        }
        qDebug() << "Opened document " << filePath;
        QTextStream textStream(&file);
        const auto maxLineLengthCharacters = 120;
        while (!textStream.atEnd()) {
            auto line = textStream.readLine(maxLineLengthCharacters);
            auto lineUtf8 = line.toUtf8();
            auto result = loader->AddData(lineUtf8.data(), lineUtf8.size());
            if (result != SC_STATUS_OK) {
                qDebug() << "Failed to add data to document " << filePath;
                return Document();
            }
        }
        qDebug() << "Finished reading document " << filePath;
        Document document(loader->ConvertToDocument());
        Q_ASSERT(document);
        releaseLoader.cancel();
        return document;
    });
    releaseLoader.cancel();
    DocumentWatcherPtr watcher(new DocumentWatcher);
    auto watcherId = watcher.get();
    QObject::connect(watcher.get(), &DocumentWatcher::finished,
                     [filePath, watcherId, this] {
        qDebug() << "Finished loading document " << filePath;
        auto id = watcherId;
        auto watcherIterator =
            std::find_if(loadingDocuments().cbegin(), loadingDocuments().cend(),
                         [id](const DocumentWatcherPtr& watcher) {
                return watcher.get() == id;
            });
        Q_ASSERT(watcherIterator != loadingDocuments().cend());
        if (watcherIterator == loadingDocuments().cend()) {
            qDebug() << "Failed to find document load watcher for " << filePath;
            return;
        }
        auto& watchers = loadingDocuments();
        ScopeExit eraseWatcher(
            [watcherIterator, &watchers] { watchers.erase(watcherIterator); });
        auto& watcher = *watcherIterator;
        auto document = watcher->future().result();
        if (!document) {
            qDebug() << "Loaded no document for " << filePath;
            return;
        }
        documents().push_back(document);
        qDebug() << "Set document for " << filePath;
        messageSender().setDocument(document);
    });
    watcher->setFuture(future);
    loadingDocuments().push_back(std::move(watcher));
    return true;
}
}