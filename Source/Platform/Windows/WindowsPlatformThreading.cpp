#include "Memory/Atomic.hpp"
#include "Utils/CString.hpp"
#include "excpt.h"
#ifdef _WIN32
#    include "Platform/Windows/WindowsPlatformThreading.hpp"
#    include "Platform/IPlatformThreading.hpp"
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#    include <dbghelp.h>

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

// Global thread-local storage for exception information
thread_local EXCEPTION_POINTERS* g_pExceptionPtrs = nullptr;

int __cdecl AccessViolationFilter(EXCEPTION_POINTERS* pExceptionPtrs) {
    g_pExceptionPtrs = pExceptionPtrs;
    return EXCEPTION_EXECUTE_HANDLER;
}

DWORD __stdcall StartManagedThread(LPVOID lpParameter) {
    const auto params = static_cast<ThreadStartParameter*>(lpParameter);
    int32_t returnValue = 0;
    __try {
        returnValue = params->ThreadFunction(params->Argument);
    } __except (AccessViolationFilter(GetExceptionInformation())) {
        const DWORD exceptionCode = GetExceptionCode();
        String reasonMessage =
            String::PrintF(u"Thread crashed with exception code: 0x%X", static_cast<uint32_t>(exceptionCode));
        CrashReason reason = CrashReason::Unknown;
        switch (exceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION: {
            reason = CrashReason::AccessViolation;
            // Provide more details for access violation including address and source location.
            if (g_pExceptionPtrs != nullptr && g_pExceptionPtrs->ExceptionRecord != nullptr) {
                PEXCEPTION_RECORD pExceptionRecord = g_pExceptionPtrs->ExceptionRecord;

                // ExceptionInformation[0]: 0 for read, 1 for write, 8 for execute
                // ExceptionInformation[1]: address of the violation
                if (pExceptionRecord->NumberParameters >= 2) {
                    ULONG_PTR accessType = pExceptionRecord->ExceptionInformation[0];
                    ULONG_PTR violationAddress = pExceptionRecord->ExceptionInformation[1];
                    const char* accessTypeStr = (accessType == 0)   ? "read"
                                                : (accessType == 1) ? "write"
                                                : (accessType == 8) ? "execute"
                                                                    : "unknown";

                    // Try to get symbol information if debugger symbols are available
                    DWORD64 moduleBase = SymGetModuleBase64(GetCurrentProcess(), (DWORD64)violationAddress);
                    if (moduleBase != 0) {
                        reasonMessage =
                            String::PrintF(u"Thread crashed due to access violation (%s) at address 0x%llX. Exception "
                                           u"code: 0x%X (Debugger symbols present)",
                                           accessTypeStr, violationAddress, static_cast<uint32_t>(exceptionCode));
                    } else {
                        reasonMessage = String::PrintF(
                            u"Thread crashed due to access violation (%s) at address 0x%llX. Exception code: 0x%X",
                            accessTypeStr, violationAddress, static_cast<uint32_t>(exceptionCode));
                    }
                } else {
                    reasonMessage = String::PrintF(u"Thread crashed due to access violation. Exception code: 0x%X",
                                                   static_cast<uint32_t>(exceptionCode));
                }
            } else {
                reasonMessage = String::PrintF(u"Thread crashed due to access violation. Exception code: 0x%X",
                                               static_cast<uint32_t>(exceptionCode));
            }
            break;
        }
        case EXCEPTION_STACK_OVERFLOW: {
            reason = CrashReason::StackOverflow;
            // Provide more details for stack overflow.
            if (g_pExceptionPtrs != nullptr && g_pExceptionPtrs->ContextRecord != nullptr) {
// Get the stack pointer from the context
#    ifdef _M_X64
                DWORD64 stackPointer = g_pExceptionPtrs->ContextRecord->Rsp;
#    elif defined(_M_IX86)
                DWORD stackPointer = g_pExceptionPtrs->ContextRecord->Esp;
#    elif defined(_M_ARM64)
                DWORD64 stackPointer = g_pExceptionPtrs->ContextRecord->Sp;
#    else
                DWORD64 stackPointer = 0;
#    endif
                reasonMessage = String::PrintF(
                    u"Thread crashed due to stack overflow at stack pointer 0x%llX. Exception code: 0x%X", stackPointer,
                    static_cast<uint32_t>(exceptionCode));
            } else {
                reasonMessage = String::PrintF(u"Thread crashed due to stack overflow. Exception code: 0x%X",
                                               static_cast<uint32_t>(exceptionCode));
            }
            break;
        }
        case EXCEPTION_FLT_DENORMAL_OPERAND:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_FLT_INEXACT_RESULT:
        case EXCEPTION_FLT_INVALID_OPERATION:
        case EXCEPTION_FLT_OVERFLOW:
        case EXCEPTION_FLT_STACK_CHECK:
        case EXCEPTION_FLT_UNDERFLOW: {
            reason = CrashReason::Unknown;
            const char* floatExceptionStr = "unknown floating-point exception";
            switch (exceptionCode) {
            case EXCEPTION_FLT_DENORMAL_OPERAND:
                floatExceptionStr = "denormal operand";
                break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                floatExceptionStr = "divide by zero";
                break;
            case EXCEPTION_FLT_INEXACT_RESULT:
                floatExceptionStr = "inexact result";
                break;
            case EXCEPTION_FLT_INVALID_OPERATION:
                floatExceptionStr = "invalid operation";
                break;
            case EXCEPTION_FLT_OVERFLOW:
                floatExceptionStr = "overflow";
                break;
            case EXCEPTION_FLT_STACK_CHECK:
                floatExceptionStr = "stack check";
                break;
            case EXCEPTION_FLT_UNDERFLOW:
                floatExceptionStr = "underflow";
                break;
            }
            reasonMessage = String::PrintF(u"Thread crashed due to floating-point exception (%s). Exception code: 0x%X",
                                           floatExceptionStr, static_cast<uint32_t>(exceptionCode));
            break;
        }
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_INT_OVERFLOW: {
            reason = CrashReason::Unknown;
            const char* intExceptionStr =
                (exceptionCode == EXCEPTION_INT_DIVIDE_BY_ZERO) ? "divide by zero" : "overflow";
            reasonMessage = String::PrintF(u"Thread crashed due to integer exception (%s). Exception code: 0x%X",
                                           intExceptionStr, static_cast<uint32_t>(exceptionCode));
            break;
        }
        case EXCEPTION_ILLEGAL_INSTRUCTION:
        case EXCEPTION_PRIV_INSTRUCTION: {
            reason = CrashReason::Unknown;
            const char* instructionStr =
                (exceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION) ? "illegal instruction" : "privileged instruction";
            reasonMessage = String::PrintF(u"Thread crashed due to %s. Exception code: 0x%X", instructionStr,
                                           static_cast<uint32_t>(exceptionCode));
            break;
        }
        case EXCEPTION_IN_PAGE_ERROR: {
            reason = CrashReason::Unknown;
            if (g_pExceptionPtrs != nullptr && g_pExceptionPtrs->ExceptionRecord != nullptr) {
                PEXCEPTION_RECORD pExceptionRecord = g_pExceptionPtrs->ExceptionRecord;
                if (pExceptionRecord->NumberParameters >= 3) {
                    ULONG_PTR faultingAddress = pExceptionRecord->ExceptionInformation[1];
                    ULONG_PTR ntstatus = pExceptionRecord->ExceptionInformation[2];
                    reasonMessage = String::PrintF(u"Thread crashed due to in-page error at address 0x%llX (NTSTATUS: "
                                                   u"0x%llX). Exception code: 0x%X",
                                                   faultingAddress, ntstatus, static_cast<uint32_t>(exceptionCode));
                } else {
                    reasonMessage = String::PrintF(u"Thread crashed due to in-page error. Exception code: 0x%X",
                                                   static_cast<uint32_t>(exceptionCode));
                }
            } else {
                reasonMessage = String::PrintF(u"Thread crashed due to in-page error. Exception code: 0x%X",
                                               static_cast<uint32_t>(exceptionCode));
            }
            break;
        }
        default:
            reason = CrashReason::Unknown;
            reasonMessage = String::PrintF(u"Thread crashed with unhandled exception code: 0x%X",
                                           static_cast<uint32_t>(exceptionCode));
            break;
        }
        params->ThreadInstance->OnThreadCrashed.Broadcast(reason, reasonMessage, *params->ThreadInstance);
    }
    WindowsPlatformThreading::UnregisterThread(params->ThreadInstance);
    delete params;
    params->ThreadInstance->PreThreadExit.Broadcast(returnValue);
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