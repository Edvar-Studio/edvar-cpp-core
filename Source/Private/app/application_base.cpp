#include "app/application_base.h"
#include "platform/platform.h"

namespace edvar::app {
application_base::application_base(string application_name, bool should_be_dpi_aware, bool register_automatically) {
    _application_name = std::move(application_name);
    if (register_automatically) {
        platform::get().register_application(as_shared());
    }

    platform::get().set_process_dpi_awareness(should_be_dpi_aware);
}
void application_base::poll_events() {
    platform::get().poll_events(*this);
}
} // namespace edvar::app