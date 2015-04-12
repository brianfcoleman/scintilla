#pragma once

#include <ScintillaEditBase.h>

#include <Document.h>
#include <ILexer.h>

#include <QPointer>

namespace cursor {
class MessageSender {
  public:
    typedef QPointer<ScintillaEditBase> ScintillaEditBasePtr;

    explicit MessageSender(const ScintillaEditBasePtr& editor)
        : m_editor(editor) {}
    ~MessageSender() {}

    ILoader* createLoader(int bytes);
    Document getDocument();
    void setDocument(Document document);
    void addRefDocument(Document document);
    void releaseDocument(Document document);

  private:
    sptr_t sendMessage(int message, uptr_t wParam = 0, sptr_t lParam = 0);
    sptr_t sendStringMessage(int message,
                             uptr_t wParam = 0,
                             const char* string = nullptr);

    const ScintillaEditBasePtr& editor() const { return m_editor; }
    ScintillaEditBasePtr& editor() { return m_editor; }

    ScintillaEditBasePtr m_editor;
};
}