#ifdef _WIN32
#    include "Platform/Windows/WindowsPlatformThreading.hpp"
#    include "Platform/IPlatformThreading.hpp"
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>

namespace Edvar::Platform::Windows {
IThread& WindowsPlatformThreading::GetCurrentThread() {
    thread_local static IThread* currentThreadPtr = nullptr;
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
    IThread* ThreadInstance;
};

DWORD __stdcall StartThread(LPVOID lpParameter) {
    const auto params = static_cast<ThreadStartParameter*>(lpParameter);
    const int32_t returnValue = params->ThreadFunction(params->Argument);
    WindowsPlatformThreading::UnregisterThread(params->ThreadInstance);
    delete params;
    return static_cast<DWORD>(returnValue);
}

IThread& WindowsPlatformThreading::CreateThread(const Containers::String& name, int32_t (*threadFunction)(void*),
                                                void* arg) {
    WindowsThread* newThread = new WindowsThread();
    auto* params = new ThreadStartParameter{threadFunction, arg, newThread};
    uint64_t threadId;
    const HANDLE threadHandle =
        ::CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)StartThread, params, CREATE_SUSPENDED, (LPDWORD)&threadId);
    newThread->NativeHandle = threadHandle;
    newThread->ThreadId = threadId;
    newThread->SetName(name);
    WindowsPlatformThreading::RegisterThread(newThread);
    ResumeThread(threadHandle);

    return *newThread;
}

void WindowsPlatformThreading::RegisterThread(IThread* thread) {
    // TODO: mutex for thread safety.
    RegisteredThreads.AddUnique(thread);
}
void WindowsPlatformThreading::UnregisterThread(IThread* thread) {
    // TODO: mutex for thread safety.
    RegisteredThreads.Remove(thread);
}

void WindowsThread::SetName(const Containers::String& name) {
    Name = name;
#    if WINVER >= _WIN32_WINNT_WIN10
    auto _ = SetThreadDescription(NativeHandle, name.CString());
#else

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
void WindowsThread::Join() {

}

} // namespace Edvar::Platform::Windows
#endif