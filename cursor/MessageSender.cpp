#include <MessageSender.h>

namespace cursor {
ILoader* MessageSender::createLoader(int bytes) {
    Q_ASSERT(bytes >= 0);
    return reinterpret_cast<ILoader*>(
        sendMessage(SCI_CREATELOADER, static_cast<int>(bytes)));
}

Document MessageSender::getDocument() {
    return Document(
        reinterpret_cast<Document::Handle>(sendMessage(SCI_GETDOCPOINTER)));
}

void MessageSender::setDocument(Document document) {
    sendMessage(SCI_SETDOCPOINTER, 0,
                reinterpret_cast<sptr_t>(document.handle()));
}

void MessageSender::addRefDocument(Document document) {
    sendMessage(SCI_ADDREFDOCUMENT, 0,
                reinterpret_cast<sptr_t>(document.handle()));
}

void MessageSender::releaseDocument(Document document) {
    sendMessage(SCI_RELEASEDOCUMENT, 0,
                reinterpret_cast<sptr_t>(document.handle()));
}

sptr_t cursor::MessageSender::sendMessage(int message,
                                          uptr_t wParam,
                                          sptr_t lParam) {
    if (editor().isNull()) {
        return 0;
    }
    return editor()->send(message, wParam, lParam);
}

sptr_t MessageSender::sendStringMessage(int message,
                                        uptr_t wParam,
                                        const char* string) {
    if (editor().isNull()) {
        return 0;
    }
    return editor()->sends(message, wParam, string);
}
}