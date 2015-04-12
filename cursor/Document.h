#pragma once

#include <vector>

namespace cursor {
class Document {
  public:
    typedef void* Handle;
    typedef Document Type;
    typedef std::vector<Type> List;

    explicit Document(Handle handle = nullptr) : m_handle(handle) {}

    Handle handle() const { return m_handle; }

    operator bool() const { return handle() != nullptr; }
    operator Handle() const { return handle(); }

  private:
    Handle m_handle;
};
}