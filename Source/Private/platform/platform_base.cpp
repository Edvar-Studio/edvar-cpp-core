#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "platform/linux/linux_platform.h"     // IWYU pragma: keep
#include "platform/windows/windows_platform.h" // IWYU pragma: keep

#ifdef _MSVC_LANG
#    define NOOP __noop()
#else
#    define NOOP (void)0
#endif

namespace edvar {

platform_base& platform_base::get() {
    static platform_type instance;
    return instance;
}

bool platform_base::is_debugger_present() { return false; }

void platform_base::debug_break(const wchar_t* message) {
    fwprintf(stderr, L"%s\n", message);
    fflush(stderr);
}

void platform_base::on_fatal(const wchar_t* message) {
    fwprintf(stderr, L"%s\n", message);
    fflush(stderr);
    exit(-1);
}

void platform_base::get_stack_trace(int skip_count, int count) { NOOP; }
} // namespace edvar