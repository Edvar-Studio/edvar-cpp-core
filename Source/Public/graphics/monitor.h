#pragma once

#include "platform/platform_windowing_system.h"
namespace edvar::graphics {
struct monitor {
    platform_windowing_system::native_monitor_handle native_handle;
};
} // namespace edvar::graphics