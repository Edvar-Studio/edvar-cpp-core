#pragma once

#include "utils/optional.h"
#include "utils/time.h"
namespace edvar {
namespace app {
struct window_style;
}
struct platform_windowing_system {
    typedef void* native_window_handle;
    typedef void* native_monitor_handle;

    virtual ~platform_windowing_system() = default;
    struct window_creation_params {
        optional<math::vector2i> size;
        optional<math::vector2i> position;
        optional<void*> owning_window_ptr;
        edvar::app::window_style* style;
        edvar::string_base<char_utf16> title;
        native_monitor_handle open_in_monitor = nullptr;
    };
    virtual value_or_error_code<native_window_handle> create_window(const window_creation_params& params) = 0;
    virtual void destroy_window(native_window_handle window) = 0;

    /**
     * @brief Called when a new window is created.
     * @param window_handle Native handle to the created window.
     */
    virtual void on_window_created(platform_windowing_system::native_window_handle window_handle) {}
    virtual void on_window_destroyed(platform_windowing_system::native_window_handle window_handle) {}
    virtual void on_window_focused(platform_windowing_system::native_window_handle window_handle) {}
    virtual void on_window_unfocused(platform_windowing_system::native_window_handle window_handle) {}
    virtual void on_window_minimized(platform_windowing_system::native_window_handle window_handle) {}
    virtual void on_window_restored(platform_windowing_system::native_window_handle window_handle) {}
    virtual void on_window_maximized(platform_windowing_system::native_window_handle window_handle) {}
    virtual void on_window_fullscreened(platform_windowing_system::native_window_handle window_handle,
                                        bool is_entering_fullscreen) {}
    virtual void on_window_moved(platform_windowing_system::native_window_handle window_handle,
                                 const math::vector2i& new_position) {}
    virtual void on_window_resized(platform_windowing_system::native_window_handle window_handle,
                                   const math::vector2i& new_size) {}
    virtual void on_window_dpi_changed(platform_windowing_system::native_window_handle window_handle,
                                       float new_dpi_scale) {}
    virtual void on_window_render_scale_changed(platform_windowing_system::native_window_handle window_handle,
                                                float new_render_scale) {}
    virtual bool on_window_close_requested(platform_windowing_system::native_window_handle window_handle,
                                           bool is_forced) {
        return true;
    }
    virtual void on_window_key_down(platform_windowing_system::native_window_handle window_handle, int repeat_count,
                                    int scan_code, bool is_extended_key, bool was_down) {}
    virtual void on_window_key_up(platform_windowing_system::native_window_handle window_handle, int scan_code,
                                  bool is_extended_key) {}
    virtual void on_window_text_input(platform_windowing_system::native_window_handle window_handle,
                                      const edvar::string_base<char_utf16>& input_text) {}
    virtual void on_window_mouse_move(platform_windowing_system::native_window_handle window_handle,
                                      const math::vector2i& new_position) {}
    virtual void on_window_mouse_button_down(platform_windowing_system::native_window_handle window_handle, int button,
                                             const math::vector2i& position) {}
    virtual void on_window_mouse_button_up(platform_windowing_system::native_window_handle window_handle, int button,
                                           const math::vector2i& position) {}
    struct touch_info {
        math::vector2f position;
        uint32_t id;
        edvar::time time_stamp;
        optional<math::vector2f> contact_size;
        bool is_down : 1;
        bool is_up : 1;
        bool is_move : 1;
    };
    virtual void on_window_touch(const array<touch_info>& touch_infos) {}
    struct pointer_event_info {
        uint64_t id;
        math::vector2f position;
        float pressure;
        edvar::time time_stamp;
        optional<math::vector2f> contact_size;
        math::vector2f tilt;
        float rotation;
        optional<float> distance;
        union {
            uint32_t buttons;
            struct {
                uint8_t first_button : 1;
                uint8_t second_button : 1;
                uint8_t third_button : 1;
                uint8_t fourth_button : 1;
                uint8_t fifth_button : 1;
            };
        };

        bool is_primary : 1;
        bool is_eraser : 1;

        bool is_predicted : 1;
        bool is_coalesced : 1;
    };
    virtual void on_window_pointer_down(platform_windowing_system::native_window_handle window_handle,
                                        const pointer_event_info& event_info) {}
    virtual void on_window_pointer_up(platform_windowing_system::native_window_handle window_handle,
                                      const pointer_event_info& event_info) {}
    virtual void on_window_pointer_move(platform_windowing_system::native_window_handle window_handle,
                                        const pointer_event_info& event_info) {}
    virtual void on_window_pointer_enter(platform_windowing_system::native_window_handle window_handle,
                                         const pointer_event_info& event_info) {}
    virtual void on_window_pointer_leave(platform_windowing_system::native_window_handle window_handle,
                                         const pointer_event_info& event_info) {}
    virtual void on_window_pointer_wheel(platform_windowing_system::native_window_handle window_handle,
                                         math::vector2i position, int wheel_delta) {}

    virtual void on_window_mouse_wheel(platform_windowing_system::native_window_handle window_handle, int wheel_delta,
                                       const math::vector2i& position) {}
    virtual void on_window_mouse_enter(platform_windowing_system::native_window_handle window_handle) {}
    virtual void on_window_mouse_leave(platform_windowing_system::native_window_handle window_handle) {}

    virtual void on_window_drag_enter(platform_windowing_system::native_window_handle window_handle,
                                      const math::vector2i& position, const array<string>& dragged_files) {}
    virtual void on_window_drag_over(platform_windowing_system::native_window_handle window_handle,
                                     const math::vector2i& position, const array<string>& dragged_files) {}
    virtual void on_window_drag_leave(platform_windowing_system::native_window_handle window_handle) {}

    virtual void on_window_drop(platform_windowing_system::native_window_handle window_handle,
                                const math::vector2i& position, const array<string>& dropped_files) {}
};
} // namespace edvar