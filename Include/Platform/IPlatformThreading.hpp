#pragma once

#include "IPlatform.hpp"
namespace Edvar::Platform {
class IThreadImplementation;
class IMutexImplementation;
class ISignalImplementation;
class ISemaphoreImplementation;

class IPlatformThreading {
public:
    virtual ~IPlatformThreading() = default;

    virtual IThreadImplementation& GetCurrentThread() = 0;
    virtual IThreadImplementation& CreateThread(const Containers::String& name, int32_t (*threadFunction)(void*),
                                                void* arg) = 0;
    virtual IMutexImplementation& CreateMutex() = 0;
    virtual ISignalImplementation& CreateSignal() = 0;
    virtual ISemaphoreImplementation& CreateSemaphore(int32_t initialCount, int32_t maxCount) = 0;
};

class IThreadImplementation {
public:
    [[nodiscard]] virtual uint64_t GetThreadId() const = 0;
    virtual void SetName(const Containers::String& name) = 0;
    [[nodiscard]] virtual Containers::String GetName() const = 0;
    [[nodiscard]] virtual bool IsCurrent() const = 0;

    virtual void SetPriority(int32_t priority) = 0;
    [[nodiscard]] virtual int32_t GetPriority() const = 0;

    [[nodiscard]] virtual bool IsAlive() const = 0;

    virtual void Join() = 0;
    virtual void Join(uint64_t milliseconds) = 0;

    virtual void Suspend() = 0;
    virtual void Resume() = 0;

    virtual void Sleep(uint32_t milliseconds) = 0;
    virtual void Yield() = 0;

    virtual void Kill(bool forceful = false) { ShouldKill = true; }

    bool ShouldBeKilled() const { return ShouldKill; }

    [[nodiscard]] static IThreadImplementation& GetCurrentThread() {
        return Platform::GetPlatform().GetThreading().GetCurrentThread();
    }

    [[nodiscard]] static IThreadImplementation& Create(const Containers::String& name, int32_t (*threadFunction)(void*),
                                                       void* arg) {
        return Platform::GetPlatform().GetThreading().CreateThread(name, threadFunction, arg);
    }

    virtual ~IThreadImplementation() = default;

    Utils::MultiDelegate<void(int returnValue)> PreThreadExit;
    Utils::MultiDelegate<void(CrashReason reason, String reasonMessage, IThreadImplementation& thread)> OnThreadCrashed;

protected:
    bool ShouldKill = false;
    virtual void ForceKill() = 0;
};

class IMutexImplementation {
public:
    virtual ~IMutexImplementation() = default;
    virtual void Lock() = 0;
    virtual void Release() = 0;
};

class ISignalImplementation {
public:
    virtual ~ISignalImplementation() = default;
    virtual void Wait(int32_t milliseconds = -1) = 0;
    virtual void NotifyOne() = 0;
    virtual void NotifyAll() = 0;
};

class ISemaphoreImplementation {
public:
    virtual ~ISemaphoreImplementation() = default;
    virtual void Wait(int32_t milliseconds = -1) = 0;
    virtual bool Signal(int32_t count = 1) = 0;
};
} // namespace Edvar::Platform