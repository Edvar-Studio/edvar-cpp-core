#include "platform/windows/windows_platform.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
namespace edvar::platform_types {
bool windows_platform::is_debugger_present() { return ::IsDebuggerPresent(); }
void windows_platform::debug_break(const wchar_t* message) {
    ::DebugBreak();
    ::OutputDebugStringW(message);
}
void windows_platform::on_fatal(const wchar_t* message) { ::OutputDebugStringW(message); FatalExit(1);}

} // namespace edvar::platform_types

#endif