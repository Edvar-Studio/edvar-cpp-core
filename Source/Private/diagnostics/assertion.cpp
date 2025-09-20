#include "diagnostics/assertion.h"
#include "platform/platform_base.h"
bool g_check_exits = false;

namespace edvar::diagnostics {
void check(const bool check_value, const wchar_t* message) {
    if (!check_value) {
        platform_base& platform = edvar::platform::get();
        if (platform.is_debugger_present()) {
            platform.debug_break(message);
        } else {
            platform.get().on_fatal(message);
        }
    }
}
} // namespace edvar::diagnostics