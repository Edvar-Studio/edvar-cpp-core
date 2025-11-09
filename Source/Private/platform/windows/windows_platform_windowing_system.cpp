#ifdef _WIN32
#    include "platform/windows/windows_platform_windowing_system.h"
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#    include "app/window.h"
namespace edvar::platform_types {

LRESULT CALLBACK windows_platform_windowing_system::window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

value_or_error_code<platform_windowing_system::native_window_handle>
windows_platform_windowing_system::create_window(const platform_windowing_system::window_creation_params& params) {
    if (params.style->get_mode() == app::window_mode::headless) {
        return value_or_error_code<platform_windowing_system::native_window_handle>::from_value(
            native_window_handle{nullptr});
    }
    static bool is_window_class_registered = false;
    static wchar_t window_class_name[] = L"EdvarCppCoreWindowClass";
    if (!is_window_class_registered) {
        WNDCLASSEXW window_class = {0};
        window_class.cbSize = sizeof(WNDCLASSEXW);
        window_class.style = CS_HREDRAW | CS_VREDRAW;
        window_class.lpfnWndProc = DefWindowProcW;
        window_class.cbClsExtra = 0;
        // allow storage of a pointer to our window instance.
        window_class.cbWndExtra = sizeof(void*);
        window_class.hInstance = GetModuleHandleW(nullptr);
        // Load the bitmap and icon resources if required
        window_class.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
        window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        window_class.lpszMenuName = nullptr;
        window_class.lpszClassName = window_class_name;
        window_class.hIconSm = window_class.hIcon; // same icon for small icon
        if (params.style->get_transparency() < 1.0f) {
            // Set layered style for transparency
            // window_class.style |= CS_DROPSHADOW;
        } else {
            window_class.style |= CS_OWNDC;
        }
        RegisterClassExW(&window_class);
        is_window_class_registered = true;
    }
    const app::window_style* style = params.style;
    DWORD window_style_flags = 0;
    DWORD window_ex_style_flags = 0;
    if (style->get_has_border()) {
        window_style_flags |= WS_BORDER;
    }
    if (style->get_has_title_bar()) {
        window_style_flags |= WS_CAPTION;
    }
    if (style->get_is_resizable()) {
        window_style_flags |= WS_SIZEBOX;
    }
    if (style->get_has_minimize_button()) {
        window_style_flags |= WS_MINIMIZEBOX;
    }
    if (style->get_has_maximize_button()) {
        window_style_flags |= WS_MAXIMIZEBOX;
    }
    if (style->get_always_on_top()) {
        window_ex_style_flags |= WS_EX_TOPMOST;
    }
    if (style->get_accept_drag_and_drop()) {
        window_ex_style_flags |= WS_EX_ACCEPTFILES;
    }
    if (style->get_has_close_button()) {
        window_style_flags |= WS_SYSMENU | WS_CAPTION;
    }
    if (!style->get_show_in_taskbar()) {
        window_ex_style_flags |= WS_EX_TOOLWINDOW;
    }
    if (style->get_transparency() < 1.0f) {
        window_ex_style_flags |= WS_EX_LAYERED;
    }

    HWND created_window = CreateWindowExW(
        window_ex_style_flags, window_class_name, reinterpret_cast<LPCWSTR>(params.title.data()), window_style_flags,
        params.position ? params.position.value().x : CW_USEDEFAULT,
        params.position ? params.position.value().y : CW_USEDEFAULT,
        params.size ? params.size.value().x : CW_USEDEFAULT, params.size ? params.size.value().y : CW_USEDEFAULT,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (created_window == nullptr) {
        return value_or_error_code<platform_windowing_system::native_window_handle>::from_error(
            static_cast<int>(GetLastError()));
    }
    if (style->get_transparency() < 1.0f) {
        BYTE alpha = static_cast<BYTE>(style->get_transparency() * 255.0f);
        if (!SetLayeredWindowAttributes(created_window, 0, alpha, LWA_ALPHA)) {
            DestroyWindow(created_window);
            return value_or_error_code<platform_windowing_system::native_window_handle>::from_error(
                static_cast<int>(GetLastError()));
        }
    }

    platform_windowing_system::native_monitor_handle monitor_handle = params.open_in_monitor;
    if (monitor_handle != nullptr) {
        // Set the window to open in the specified monitor
        SetWindowLongPtrW(created_window, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(monitor_handle));
        // Set the relative position if provided
        if (params.position.has_value()) {
            RECT monitor_rect;
            GetWindowRect(reinterpret_cast<HWND>(monitor_handle), &monitor_rect);
            int new_x = monitor_rect.left + params.position->x;
            int new_y = monitor_rect.top + params.position->y;
            SetWindowPos(created_window, nullptr, new_x, new_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        }
    }
    // sets the class extra bytes to point to our window instance.
    if (params.owning_window_ptr.has_value()) {
        SetWindowLongPtrW(created_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(params.owning_window_ptr.value()));
    }

    if (style->get_mode() == app::window_mode::hidden) {
        ShowWindow(created_window, SW_MINIMIZE);
    } else if (style->get_mode() == app::window_mode::borderless) {
        ShowWindow(created_window, SW_SHOW);
        SetWindowLongW(created_window, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        SetWindowPos(created_window, nullptr, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                     SWP_NOZORDER | SWP_FRAMECHANGED);
    } else {
        ShowWindow(created_window, SW_SHOW);
    }

    // TODO: Setup the rendering context here.

    return value_or_error_code<platform_windowing_system::native_window_handle>::from_value(
        native_window_handle{created_window});
}
} // namespace edvar::platform_types
#endif