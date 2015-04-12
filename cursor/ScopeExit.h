#pragma once

#include <functional>

namespace cursor {
class ScopeExit {
  public:
    template <typename TCallable>
    explicit ScopeExit(TCallable callable) : m_callable(callable) {}
    ~ScopeExit() { m_callable(); }

  private:
    ScopeExit(const ScopeExit&);
    ScopeExit& operator=(const ScopeExit&);

    std::function<void()> m_callable;
};
}