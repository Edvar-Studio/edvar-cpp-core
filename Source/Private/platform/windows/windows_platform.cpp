#include "combaseapi.h"
#ifdef _WIN32
#    define WIN32_LEAN_AND_MEAN
#    include "platform/windows/windows_platform.h"
#    include "platform/platform_base.h"
#    include <windows.h>
#    include <winnt.h>
#    include <stdio.h>
#    include <DbgHelp.h>
#    include <libloaderapi.h>
#    include <Psapi.h>
#    include <winerror.h>
#    include <objbase.h>
namespace edvar::platform_types {
bool windows_platform::is_debugger_present() { return ::IsDebuggerPresent(); }
void windows_platform::debug_break(const wchar_t* message) {
    ::DebugBreak();
    ::OutputDebugStringW(message);
}
void windows_platform::on_fatal(const wchar_t* message) {
    ::OutputDebugStringW(message);
    ::FatalExit(0xFFFF);
}

pair<bool, edvar::string_utf16> find_function_name(uint64 address) {
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, nullptr, true);
    // Initialize the SYMBOL_INFO structure
    DWORD64 displacement = 0;
    uint8 buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(buffer);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    if (SymFromAddr(process, address, &displacement, symbol)) {
        // Convert the function name to char_utf16
        size_t name_length = strlen(symbol->Name);
        return {true, symbol->Name};
    } else {
        return {false, "Unknown Function"};
    }
}

edvar::string_base<char_utf16> edvar::platform_types::windows_platform::get_stack_trace(int skip_count, int count) {
    void** stack = new void*[count];
    USHORT frames = CaptureStackBackTrace(skip_count, count, stack, NULL);
    // Print the stack trace to the debug output
    string_base<char_utf16> result;
    for (USHORT i = 0; i < frames; i++) {
        // If pdb is available, it should show the function names as well.
        // Otherwise, it will just show the addresses.
        pair<bool, edvar::string_utf16> func_name = find_function_name(reinterpret_cast<uint64>(stack[i]));
        result += string::format("{}\t\t{}\n", stack[i], func_name.value());
    }
    delete[] stack;
    return result;
}

void windows_platform::abort(int32 exit_code, const process_handle& in_process) {
    ::TerminateProcess(reinterpret_cast<HANDLE>(in_process ? in_process : GetCurrentProcess()),
                       static_cast<UINT>(exit_code));
}
bool windows_platform::is_valid_process(const process_handle& in_process) {
    if (!in_process) {
        return false;
    }
    DWORD exit_code;
    if (!GetExitCodeProcess(reinterpret_cast<HANDLE>(in_process), &exit_code)) {
        GetLastError(); // The process handle is not allowed to be inspected (at least for this call). So we should
                        // report it as an invalid handle and ignore the last error.
        return false;
    }
    return exit_code == STILL_ACTIVE;
}

edvar::string_base<char_utf16>& windows_platform::get_process_name(const process_handle& in_process) {
    if (!in_process) {
        return get_process_name(get_this_process_handle());
    }
    static edvar::container::map<process_handle, edvar::string_base<char_utf16>> process_name_cache;
    if (process_name_cache.contains(in_process)) {
        return process_name_cache[in_process];
    }
    wchar_t buffer[MAX_PATH + 1];
    DWORD size = GetProcessImageFileNameW(reinterpret_cast<HANDLE>(in_process), buffer, MAX_PATH);
    buffer[MAX_PATH] = L'\0'; // make sure there is a null terminator
    process_name_cache.insert(in_process, edvar::string_base<char_utf16>(buffer));
    if (size > 0) {
        return process_name_cache[in_process];
    }
    static edvar::string_base<char_utf16> unknown_process = L"Unknown Process";
    return unknown_process;
}

edvar::string_base<char_utf16> windows_platform::get_process_executable_path(const process_handle& in_process) {
    if (!in_process) {
        return get_process_executable_path(get_this_process_handle());
    }
    wchar_t buffer[MAX_PATH + 1];
    DWORD size = GetModuleFileNameExW(reinterpret_cast<HANDLE>(in_process), NULL, buffer, MAX_PATH);
    buffer[MAX_PATH] = L'\0'; // make sure there is a null terminator
    if (size > 0) {
        return edvar::string_base<char_utf16>(buffer);
    }
    return L"";
}
edvar::string_base<char_utf16> windows_platform::get_start_arguments_string() {
    LPWSTR command_line = GetCommandLineW();
    return edvar::string_base<char_utf16>(command_line);
}
windows_platform::windows_platform() {
    CoInitializeEx(nullptr, COINIT::COINIT_APARTMENTTHREADED | COINIT::COINIT_DISABLE_OLE1DDE);
}
windows_platform::~windows_platform() { CoUninitialize(); }
} // namespace edvar::platform_types
#endif