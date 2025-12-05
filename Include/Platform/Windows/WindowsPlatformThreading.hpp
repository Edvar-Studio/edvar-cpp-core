#pragma once

#include "Memory/Atomic.hpp"
#ifdef _WIN32
#    include "Platform/IPlatformThreading.hpp"

namespace Edvar::Platform::Windows {

class WindowsPlatformThreading : public IPlatformThreading {
public:
    WindowsPlatformThreading();
    virtual IThreadImplementation& GetCurrentThread() override;
    virtual IThreadImplementation& CreateThread(const Containers::String& name, int32_t (*threadFunction)(void*),
                                                void* arg) override;
    virtual IMutexImplementation& CreateMutex() override;
    virtual ISignalImplementation& CreateSignal() override;

    static void RegisterThread(IThreadImplementation* thread);
    static void UnregisterThread(IThreadImplementation* thread);
    static IMutexImplementation* RegisteredThreadsMutex;

private:
    static Containers::List<IThreadImplementation*> RegisteredThreads;
};

class WindowsThread final : public IThreadImplementation {
public:
    WindowsThread(void* InNativeHandle, uint64_t InThreadId) : NativeHandle(InNativeHandle), ThreadId(InThreadId) {}
    WindowsThread() : NativeHandle(nullptr), ThreadId(0) {}
    ~WindowsThread() override;
    [[nodiscard]] uint64_t GetThreadId() const override { return ThreadId; }
    void SetName(const Containers::String& name) override;
    [[nodiscard]] Containers::String GetName() const override { return Name; }
    [[nodiscard]] bool IsCurrent() const override {
        return GetThreadId() == IThreadImplementation::GetCurrentThread().GetThreadId();
    }

    void SetPriority(int32_t priority) override;
    [[nodiscard]] int32_t GetPriority() const override { return Priority; }

    [[nodiscard]] bool IsAlive() const override;

    void Join() override;
    void Join(uint64_t milliseconds) override;

    void Sleep(uint32_t milliseconds) override;
    void Yield() override;
    void Suspend() override;
    void Resume() override;

    void ForceKill() override;

    void* NativeHandle;
    uint64_t ThreadId;
    String Name;
    int Priority = 0;

};

class WindowsMutex final : public IMutexImplementation {
public:
    ~WindowsMutex() override;
    void Lock() override;
    void Release() override;

    void* NativeHandle;
};

class WindowsSignal final : public ISignalImplementation {
public:
    ~WindowsSignal() override;
    WindowsSignal();
    void Wait(int32_t milliseconds = -1) override;

    void NotifyOne() override;
    void NotifyAll() override;

    void* NativeHandle;
    Memory::Atomic<uint64_t> Generation;
};

class WindowsSemaphore final : public ISemaphoreImplementation {
public:
    ~WindowsSemaphore() override;
    WindowsSemaphore(int32_t initialCount, int32_t maxCount);
    void Wait(int32_t milliseconds = -1) override;

    bool Signal(int32_t releaseCount = 1) override;

    void* NativeHandle;
};
} // namespace Edvar::Platform::Windows
#endif