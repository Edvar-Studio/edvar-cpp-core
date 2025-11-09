#include "app/window.h"
#include "graphics/monitor.h"
#include "memory/smart_pointers.h"
#include "platform/platform_windowing_system.h"

namespace edvar::app {
memory::shared_pointer<window> window::create(string title, const optional<math::vector2i>& size,
                                              const optional<math::vector2i>& position, window_style style,
                                              graphics::monitor* open_in_monitor) {
    memory::shared_pointer<window> new_window = memory::shared_pointer<window>(new window());
    new_window->_title = title;
    new_window->_size = size.value_or(math::vector2i(800, 600));
    new_window->_position = position.value_or(math::vector2i(100, 100));
    new_window->_style = style;

    platform_windowing_system::window_creation_params params;
    params.title = title;
    params.size = size;
    params.position = position;
    params.style = &style;
    params.open_in_monitor = open_in_monitor ? open_in_monitor->native_handle : nullptr;
    params.owning_window_ptr = reinterpret_cast<void*>(new_window.get());

    value_or_error_code<platform_windowing_system::native_window_handle> result =
        platform::get().get_windowing_system().create_window(params);

    return new_window;
}
} // namespace edvar::app