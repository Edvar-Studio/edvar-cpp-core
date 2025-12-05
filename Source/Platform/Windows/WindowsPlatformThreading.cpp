#include "Memory/Atomic.hpp"
#include "Utils/CString.hpp"
#ifdef _WIN32
#    include "Platform/Windows/WindowsPlatformThreading.hpp"
#    include "Platform/IPlatformThreading.hpp"
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>

namespace Edvar::Platform::Windows {
IThreadImplementation& WindowsPlatformThreading::GetCurrentThread() {
    thread_local static IThreadImplementation* currentThreadPtr = nullptr;
    if (currentThreadPtr == nullptr) {
        HANDLE currentThreadHandle = ::GetCurrentThread();
        DWORD threadId = ::GetCurrentThreadId();
        currentThreadPtr = new WindowsThread(currentThreadHandle, static_cast<uint64_t>(threadId));
    }
    return *currentThreadPtr;
}

struct ThreadStartParameter {
    int32_t (*ThreadFunction)(void*);
    void* Argument;
    IThreadImplementation* ThreadInstance;
};

DWORD __stdcall StartManagedThread(LPVOID lpParameter) {
    const auto params = static_cast<ThreadStartParameter*>(lpParameter);
    const int32_t returnValue = params->ThreadFunction(params->Argument);
    WindowsPlatformThreading::UnregisterThread(params->ThreadInstance);
    delete params;
    return static_cast<DWORD>(returnValue);
}

IThreadImplementation& WindowsPlatformThreading::CreateThread(const Containers::String& name,
                                                              int32_t (*threadFunction)(void*), void* arg) {
    WindowsThread* newThread = new WindowsThread();
    auto* params = new ThreadStartParameter{threadFunction, arg, newThread};
    uint64_t threadId;
    const HANDLE threadHandle = ::CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)StartManagedThread, params,
                                               CREATE_SUSPENDED, (LPDWORD)&threadId);
    newThread->NativeHandle = threadHandle;
    newThread->ThreadId = threadId;
    newThread->SetName(name);
    WindowsPlatformThreading::RegisterThread(newThread);
    ResumeThread(threadHandle);

    return *newThread;
}
#    pragma push_macro("CreateMutex")
#    undef CreateMutex
IMutexImplementation& WindowsPlatformThreading::CreateMutex() {
#    pragma pop_macro("CreateMutex")
    CRITICAL_SECTION* mutex = new CRITICAL_SECTION;
    InitializeCriticalSection(mutex);
    WindowsMutex* newMutex = new WindowsMutex();
    newMutex->NativeHandle = mutex;
    return *newMutex;
}

IMutexImplementation* WindowsPlatformThreading::RegisteredThreadsMutex = nullptr;
Containers::List<IThreadImplementation*> WindowsPlatformThreading::RegisteredThreads;
void WindowsPlatformThreading::RegisterThread(IThreadImplementation* thread) {
    Threading::ScopedLock lock(*RegisteredThreadsMutex);
    RegisteredThreads.AddUnique(thread);
}
void WindowsPlatformThreading::UnregisterThread(IThreadImplementation* thread) {
    Threading::ScopedLock lock(*RegisteredThreadsMutex);
    RegisteredThreads.Remove(thread);
}

void WindowsThread::SetName(const Containers::String& name) {
    Name = name;
#    if WINVER >= _WIN32_WINNT_WIN10
    wchar_t* wName = new wchar_t[name.Length() + 1];
    Utils::CStrings::ToWCharString(name.Data(), wName, name.Length() + 1);
    auto _ = SetThreadDescription(NativeHandle, wName);
    delete[] wName;
#    endif
}

void WindowsThread::SetPriority(int32_t priority) {
    Priority = priority;
    ::SetThreadPriority(NativeHandle, priority);
}
bool WindowsThread::IsAlive() const {
    if (NativeHandle == nullptr || NativeHandle == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD state;
    ::GetExitCodeThread(NativeHandle, &state);
    return state != STILL_ACTIVE;
}
void WindowsThread::Join() { Join(INFINITE); }
void WindowsThread::Join(const uint64_t milliseconds) { WaitForSingleObject(NativeHandle, milliseconds); }
void WindowsThread::Sleep(const uint32_t milliseconds) { ::Sleep(milliseconds); }
#    pragma push_macro("Yield")
#    undef Yield
void WindowsThread::Yield() { _mm_pause(); }
#    pragma pop_macro("Yield")

void WindowsThread::Suspend() { ::SuspendThread(NativeHandle); }
void WindowsThread::Resume() { ::ResumeThread(NativeHandle); }

WindowsMutex::~WindowsMutex() { ::DeleteCriticalSection(static_cast<LPCRITICAL_SECTION>(NativeHandle)); }
void WindowsMutex::Lock() { ::TryEnterCriticalSection(static_cast<LPCRITICAL_SECTION>(NativeHandle)); }
void WindowsMutex::Release() { ::LeaveCriticalSection(static_cast<LPCRITICAL_SECTION>(NativeHandle)); }

WindowsPlatformThreading::WindowsPlatformThreading() {
#    pragma push_macro("CreateMutex")
#    undef CreateMutex
    RegisteredThreadsMutex = &CreateMutex();
#    pragma pop_macro("CreateMutex")
}
WindowsSignal::WindowsSignal() {
    NativeHandle = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    Generation.Store(0, Memory::MemoryOrder::Relaxed);
}
WindowsSignal::~WindowsSignal() { CloseHandle(static_cast<HANDLE>(NativeHandle)); }
void WindowsSignal::Wait(int32_t milliseconds) {
    uint64_t myGen = Generation.Load(Memory::MemoryOrder::Acquire);
    for (;;) {
        uint64_t currentGen = Generation.Load(Memory::MemoryOrder::Acquire);
        if (currentGen != myGen) {
            return;
        }
        DWORD timeOut = (milliseconds < 0) ? INFINITE : static_cast<DWORD>(milliseconds);
        DWORD r = WaitForSingleObject(static_cast<HANDLE>(NativeHandle), timeOut);
        if (r == WAIT_TIMEOUT) {
            return;
        }
    }
}
void WindowsSignal::NotifyOne() {
    Generation.FetchAdd(1, Memory::MemoryOrder::AcquireAndRelease);
    SetEvent(static_cast<HANDLE>(NativeHandle));
    ResetEvent(static_cast<HANDLE>(NativeHandle));
}
void WindowsSignal::NotifyAll() {
    Generation.FetchAdd(1, Memory::MemoryOrder::AcquireAndRelease);
    SetEvent(static_cast<HANDLE>(NativeHandle));
    ResetEvent(static_cast<HANDLE>(NativeHandle));
}
ISignalImplementation& WindowsPlatformThreading::CreateSignal() {
    WindowsSignal* newSignal = new WindowsSignal();
    return *newSignal;
}
WindowsSemaphore::~WindowsSemaphore() { CloseHandle(static_cast<HANDLE>(NativeHandle)); }
WindowsSemaphore::WindowsSemaphore(int32_t initialCount, int32_t maxCount) {
    NativeHandle = CreateSemaphoreW(nullptr, initialCount, maxCount, nullptr);
}
void WindowsSemaphore::Wait(int32_t milliseconds) {
    DWORD timeOut = (milliseconds < 0) ? INFINITE : static_cast<DWORD>(milliseconds);
    WaitForSingleObject(static_cast<HANDLE>(NativeHandle), timeOut);
}
bool WindowsSemaphore::Signal(int32_t releaseCount) {
    return ReleaseSemaphore(static_cast<HANDLE>(NativeHandle), releaseCount, nullptr);
}
WindowsThread::~WindowsThread() {
    if (NativeHandle != nullptr && NativeHandle != INVALID_HANDLE_VALUE) {
        if (IsAlive()) {
            Kill(false);
        }
        CloseHandle(static_cast<HANDLE>(NativeHandle));
        NativeHandle = nullptr;
    }
}
void WindowsThread::ForceKill() {
    if (NativeHandle != nullptr && NativeHandle != INVALID_HANDLE_VALUE) {
        TerminateThread(static_cast<HANDLE>(NativeHandle), 1);
        CloseHandle(static_cast<HANDLE>(NativeHandle));
        NativeHandle = nullptr;
    }
}
} // namespace Edvar::Platform::Windows
#endif