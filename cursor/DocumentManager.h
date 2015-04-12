#pragma once

#include <Document.h>

#include <QFutureWatcher>
#include <QString>

#include <memory>
#include <vector>

namespace cursor {
class MessageSender;

class DocumentManager {
  public:
    explicit DocumentManager(MessageSender& messageSender)
        : m_messageSender(messageSender) {}
    ~DocumentManager() {}

    bool loadDocument(const QString& filePath);

  private:
    DocumentManager(const DocumentManager&);
    DocumentManager& operator=(const DocumentManager&);

    const MessageSender& messageSender() const { return m_messageSender; }
    MessageSender& messageSender() { return m_messageSender; }

    typedef QFutureWatcher<Document> DocumentWatcher;
    typedef std::unique_ptr<DocumentWatcher> DocumentWatcherPtr;
    typedef std::vector<DocumentWatcherPtr> DocumentWatcherList;

    const DocumentWatcherList& loadingDocuments() const {
        return m_loadingDocuments;
    }
    DocumentWatcherList& loadingDocuments() { 
        return m_loadingDocuments; 
    }

    const Document::List& documents() const { return m_documents; }
    Document::List& documents() { return m_documents; }

    MessageSender& m_messageSender;
    DocumentWatcherList m_loadingDocuments;
    Document::List m_documents;
};
}