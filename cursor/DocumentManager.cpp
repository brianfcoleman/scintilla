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
#include <memory>

namespace cursor {
bool DocumentManager::loadDocument(const QString& filePath) {
    typedef std::shared_ptr<ILoader> ILoaderPtr;
    const int initialDocumentSizeBytes = 1024;
    ILoaderPtr loader(messageSender().createLoader(initialDocumentSizeBytes),
                      [](ILoader* loader) {
        if (loader == nullptr) {
            return;
        }
        loader->Release();
    });
    if (!loader) {
        return false;
    }
    auto future = QtConcurrent::run([filePath, loader] () -> Document {
        using namespace cursor;
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            return Document();
        }
        QTextStream textStream(&file);
        const auto maxLineLengthCharacters = 120;
        while (!textStream.atEnd()) {
            auto line = textStream.readLine(maxLineLengthCharacters);
            auto lineUtf8 = line.toUtf8();
            auto result = loader->AddData(lineUtf8.data(), lineUtf8.size());
            if (result != SC_STATUS_OK) {
                return Document();
            }
        }
        Document document(loader->ConvertToDocument());
        return document;
    });
    DocumentWatcherPtr watcher(new DocumentWatcher);
    auto watcherId = watcher.get();
    QObject::connect(watcher.get(), &DocumentWatcher::finished,
                     [watcherId, this] {
        auto id = watcherId;
        auto watcherIterator =
            std::find_if(loadingDocuments().cbegin(), loadingDocuments().cend(),
                         [id](const DocumentWatcherPtr& watcher) {
                return watcher.get() == id;
            });
        Q_ASSERT(watcherIterator != loadingDocuments().cend());
        if (watcherIterator != loadingDocuments().cend()) {
            return;
        }
        auto& watchers = loadingDocuments();
        ScopeExit eraseWatcher(
            [watcherIterator, &watchers] { watchers.erase(watcherIterator); });
        auto& watcher = *watcherIterator;
        auto document = watcher->future().result();
        if (!document) {
            return;
        }
        documents().push_back(document);
        messageSender().setDocument(document);
    });
    watcher->setFuture(future);
    loadingDocuments().push_back(std::move(watcher));
    return true;
}
}