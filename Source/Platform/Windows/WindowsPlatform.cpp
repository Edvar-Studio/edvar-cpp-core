#include "Platform/Windows/WindowsPlatform.hpp"
#ifdef _WIN32
#    include "Platform/Windows/WindowsPlatformThreading.hpp"
#    include "Platform/Windows/WindowsPlatformWindowing.hpp"
#    include "Platform/Windows/WindowsPlatformInput.hpp"

#    include <windows.h>

namespace Edvar::Platform::Windows {
IPlatformThreading& WindowsPlatform::GetThreading() const {
    static WindowsPlatformThreading threading;
    return threading;
}

IPlatformWindowing& WindowsPlatform::GetWindowing() const {
    static WindowsPlatformWindowing windowing;
    return windowing;
}

IPlatformInput& WindowsPlatform::GetInput() const {
    static WindowsPlatformInput input;
    return input;
}
void WindowsPlatform::PrintMessageToDebugger(const char16_t* message) {
    if (IsDebuggerPresent()) {
        const int32_t bufferSize = Utils::CStrings::ToWCharString(message, nullptr, 0) + 1;
        auto* buffer = new wchar_t[bufferSize];
        Utils::CStrings::ToWCharString(message, buffer, bufferSize);
        OutputDebugStringW(buffer);
        delete[] buffer;
    }
}
} // namespace Edvar::Platform::Windows
#endif