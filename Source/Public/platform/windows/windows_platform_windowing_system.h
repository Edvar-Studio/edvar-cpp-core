#pragma once

#include "force_include.h"
#ifdef _WIN32
#    include "../platform_windowing_system.h"
#    include "platform/platform_windowing_system.h"
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>

namespace edvar::platform_types {
struct windows_platform_windowing_system : public platform_windowing_system {
    virtual value_or_error_code<native_window_handle> create_window(const window_creation_params& params) override;


private:

    static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
} // namespace edvar::platform_types
#endif