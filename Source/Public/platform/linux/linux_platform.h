#pragma once
#ifdef __linux__

#include "platform/platform_base.h"

namespace edvar::platform_types {
class linux_platform : platform_base {
#include "../generic_type_usage"
};
} // namespace edvar

#endif