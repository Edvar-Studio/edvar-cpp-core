#pragma once

#include "math/vector.h"
#include "utils/optional.h"
#define WINDOW_STYLE_ARGUMENT(type, name)                                                                              \
private:                                                                                                               \
    type _##name;                                                                                                      \
                                                                                                                       \
public:                                                                                                                \
    window_style& name(const type& in_value) {                                                                         \
        _##name = in_value;                                                                                            \
        return *this;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
public:                                                                                                                \
    type get_##name() const { return _##name; }
namespace edvar::graphics {
struct monitor;
}
namespace edvar::app {

enum class window_mode : uint8_t { windowed, borderless, fullscreen, hidden, headless };

struct window_style {

    WINDOW_STYLE_ARGUMENT(bool, is_resizable)
    WINDOW_STYLE_ARGUMENT(bool, has_title_bar)
    WINDOW_STYLE_ARGUMENT(bool, has_minimize_button)
    WINDOW_STYLE_ARGUMENT(bool, has_maximize_button)
    WINDOW_STYLE_ARGUMENT(bool, has_close_button)
    WINDOW_STYLE_ARGUMENT(bool, has_border)
    WINDOW_STYLE_ARGUMENT(float, transparency)
    WINDOW_STYLE_ARGUMENT(bool, vsync)
    WINDOW_STYLE_ARGUMENT(bool, high_dpi)
    WINDOW_STYLE_ARGUMENT(bool, focus_on_show)
    WINDOW_STYLE_ARGUMENT(bool, always_on_top)
    WINDOW_STYLE_ARGUMENT(bool, accept_drag_and_drop)
    WINDOW_STYLE_ARGUMENT(bool, show_in_taskbar)
    WINDOW_STYLE_ARGUMENT(window_mode, mode)
    WINDOW_STYLE_ARGUMENT(bool, hdr)

#undef WINDOW_STYLE_ARGUMENT

    static window_style default_style() {
        return window_style()
            .is_resizable(true)
            .has_title_bar(true)
            .has_minimize_button(true)
            .has_maximize_button(true)
            .has_close_button(true)
            .has_border(true)
            .transparency(1.f)
            .vsync(true)
            .high_dpi(true)
            .focus_on_show(true)
            .always_on_top(false)
            // Disable drag and drop by default for security reasons
            .accept_drag_and_drop(false)
            .show_in_taskbar(true)
            .mode(window_mode::windowed)
            .hdr(true);
    }
};
class window : memory::enable_shared_from_this<window> {
    static memory::shared_pointer<window> create(string title,
                                                 const optional<math::vector2i>& size = optional<math::vector2i>(),
                                                 const optional<math::vector2i>& position = optional<math::vector2i>(),
                                                 window_style style = window_style::default_style(),
                                                 graphics::monitor* open_in_monitor = nullptr);

protected:
    ~window();
    window();
    friend struct ::edvar::platform_windowing_system;

private:
    string _title;
    math::vector2i _size;
    math::vector2i _position;
    window_style _style;
};
} // namespace edvar::app