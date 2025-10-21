// Minimal platform_base translation unit: only provide the platform singleton.
#include "platform/platform.h" // defines platform_type and platform selection
#include "platform/platform_base.h"

namespace edvar {

platform_base& platform_base::get() { static platform_type instance; return instance; }

} // namespace edvar