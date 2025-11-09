// Minimal platform_base translation unit: only provide the platform singleton.
#include "platform/platform.h" // defines platform_type and platform selection
#include "platform/platform_base.h"

namespace edvar {

platform_base& platform_base::get() {
    static platform_type instance;
    return instance;
}

void platform_base::register_application(memory::shared_pointer<edvar::app::application_base> application) {
    if (!application)
        return;
    if (application_registry.contains(application)) {
        // already registered
        return;
    }
    application_registry.add(application);
}
void platform_base::unregister_application(memory::shared_pointer<edvar::app::application_base> application) {
    if (!application)
        return;
    auto found = application_registry.find(application);
    if (found.is_error()) {
        // not found
        return;
    }
    application_registry.remove(application.get());
}
} // namespace edvar