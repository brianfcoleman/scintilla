#pragma once

#include <functional>

namespace cursor {
class ScopeExit {
  public:
    template <typename TCallable>
    explicit ScopeExit(TCallable callable)
        : m_callable(callable), m_isCancelled(false) {}
    ~ScopeExit() {
        if (m_isCancelled) {
            return;
        }
        m_callable();
    }

    void cancel() { m_isCancelled = true; }

  private:
    ScopeExit(const ScopeExit&);
    ScopeExit& operator=(const ScopeExit&);

    std::function<void()> m_callable;
    bool m_isCancelled;
};
}