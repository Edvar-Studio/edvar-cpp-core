#pragma once

namespace Edvar::Threading {
class Signal {
public:
    Signal();
    ~Signal();

    void Wait(int32_t milliseconds = -1);
    void NotifyOne();
    void NotifyAll();
};
} // namespace Edvar::Threading