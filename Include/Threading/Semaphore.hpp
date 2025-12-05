#pragma once

#include "Memory/Atomic.hpp"
#include "Platform/IPlatformThreading.hpp"
namespace Edvar::Threading {
class Semaphore {
public:
    Semaphore(int32_t initialCount, int32_t maxCount) {
        implementation = &Platform::Get().GetThreading().CreateSemaphore(initialCount, maxCount);
    }
    ~Semaphore() { delete implementation; }

    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore(Semaphore&& other) noexcept : implementation(other.implementation) { other.implementation = nullptr; }
    Semaphore& operator=(Semaphore&& other) noexcept {
        if (this != &other) {
            delete implementation;
            implementation = other.implementation;
            other.implementation = nullptr;
        }
        return *this;
    }

    void Wait(int32_t milliseconds = -1) {
        implementation->Wait(milliseconds);
        currentCount.FetchAdd(-1, Memory::MemoryOrder::Relaxed);
    }
    void Signal(int32_t releaseCount = 1) {
        int32_t oldCount = currentCount.FetchAdd(releaseCount, Memory::MemoryOrder::Relaxed);
        if (!implementation->Signal(releaseCount)) {
            currentCount.Store(oldCount, Memory::MemoryOrder::Relaxed);
        }
    }

    int32_t GetCurrentCount() const { return currentCount.Load(Memory::MemoryOrder::Relaxed); }

private:
    Platform::ISemaphoreImplementation* implementation;
    Memory::Atomic<int32_t> currentCount;
};
} // namespace Edvar::Threading