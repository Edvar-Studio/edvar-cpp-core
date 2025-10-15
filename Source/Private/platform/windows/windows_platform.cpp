#include "platform/windows/windows_platform.h"
#ifdef _WIN32
#    define WIN32_LEAN_AND_MEAN
#    include <Windows.h>
#    include <stdio.h>
#    include <DbgHelp.h>
namespace edvar::platform_types {
bool windows_platform::is_debugger_present() { return ::IsDebuggerPresent(); }
void windows_platform::debug_break(const wchar_t* message) {
    ::DebugBreak();
    ::OutputDebugStringW(message);
}
void windows_platform::on_fatal(const wchar_t* message) {
    ::OutputDebugStringW(message);
    FatalExit(1);
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
} // namespace edvar::platform_types

#endif