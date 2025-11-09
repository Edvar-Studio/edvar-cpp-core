#pragma once

#include "memory/smart_pointers.h"
#include "platform/platform_windowing_system.h"
namespace edvar::graphics {
class swapchain : public memory::enable_shared_from_this<swapchain> {
public:
    static memory::shared_pointer<swapchain> create(platform_windowing_system::native_window_handle window_handle);

protected:
    platform_windowing_system::native_window_handle _window;
    swapchain() = default;
    virtual ~swapchain() = default;
};
} // namespace edvar::graphics