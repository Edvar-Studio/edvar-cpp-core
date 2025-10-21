#pragma once

namespace edvar::platform_types {
struct platform_windowing_system {
    typedef void* native_window_handle;
    typedef void* native_display_handle;
    
    virtual ~platform_windowing_system() = default;
};
} // namespace edvar::platform_types