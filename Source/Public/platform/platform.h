#pragma once
#if defined(_WIN32)
#include "windows/windows_platform.h"
#elif defined(__linux__)
#include "linux/linux_platform.h"
#endif

namespace edvar {

#if defined(_WIN32)
using platform_type = edvar::platform_types::windows_platform;
#elif defined(__linux__)
using platform_type = edvar::platform_types::linux_platform;
#endif

namespace platform {
class platform_base& get() { return platform_base::get(); }
} // namespace platform

} // namespace edvar