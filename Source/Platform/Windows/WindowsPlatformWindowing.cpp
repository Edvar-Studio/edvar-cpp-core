#include "Platform/Windows/WindowsPlatformWindowing.hpp"
#include "Platform/Windows/WindowsPlatformInput.hpp"
#include "Windowing/Window.hpp"
#define WIN32_LEAN_AND_MEAN
#include "Rendering/RenderingCore.hpp"

#include <windows.h>
#include <windowsx.h>
#include <shellscalingapi.h>

// Link with required libraries
#pragma comment(lib, "Shcore.lib")

namespace Edvar::Platform::Windows {

// Global map to store window instances for message routing
static Containers::List<WindowsWindow*> g_WindowInstances;
static Threading::Mutex g_WindowInstancesMutex;

// Forward declarations
static LRESULT CALLBACK GlobalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ============================================================================
// WindowsPlatformWindowing Implementation
// ============================================================================

WindowsPlatformWindowing::WindowsPlatformWindowing() {
    // Set DPI awareness to per-monitor V2
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    UpdatePrimaryMonitor();
}

WindowsPlatformWindowing::~WindowsPlatformWindowing() {
    // Cleanup is handled by individual window destructors
}

Containers::List<MonitorInfo> WindowsPlatformWindowing::GetMonitors() {
    Containers::List<MonitorInfo> monitors;

    // Enumerate all monitors
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) -> BOOL {
            auto* monitors = reinterpret_cast<Containers::List<MonitorInfo>*>(lParam);
            MonitorInfo info = CreateMonitorInfo(hMonitor);
            monitors->Push(info);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&monitors));

    return monitors;
}

const MonitorInfo& WindowsPlatformWindowing::GetPrimaryMonitor() { return PrimaryMonitor; }
#pragma push_macro("CreateWindow")
#undef CreateWindow
IWindowImplementation& WindowsPlatformWindowing::CreateWindow(const Windowing::WindowDescriptor& descriptor) {
#pragma pop_macro("CreateWindow")
    if (!WindowClassRegistered) {
        RegisterWindowClass();
    }

    // Determine window style
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exStyle = WS_EX_APPWINDOW;

    if (descriptor.Mode == Windowing::WindowMode::Borderless) {
        style = WS_POPUP;
    } else if (descriptor.Mode == Windowing::WindowMode::Fullscreen) {
        style = WS_POPUP;
        exStyle |= WS_EX_TOPMOST;
    } else {
        // Apply window style flags
        style = WS_OVERLAPPED;
        if (static_cast<uint32_t>(descriptor.Style & Windowing::WindowStyle::Titlebar)) {
            style |= WS_CAPTION;
        }
        if (static_cast<uint32_t>(descriptor.Style & Windowing::WindowStyle::Resizable)) {
            style |= WS_THICKFRAME;
        }
        if (static_cast<uint32_t>(descriptor.Style & Windowing::WindowStyle::Minimizable)) {
            style |= WS_MINIMIZEBOX;
        }
        if (static_cast<uint32_t>(descriptor.Style & Windowing::WindowStyle::Maximizable)) {
            style |= WS_MAXIMIZEBOX;
        }
        if (static_cast<uint32_t>(descriptor.Style & Windowing::WindowStyle::Closable)) {
            style |= WS_SYSMENU;
        }
    }

    if (descriptor.Visible) {
        style |= WS_VISIBLE;
    }
    const Math::Vector2i position = descriptor.Position.GetOrDefault(Math::Vector2i(CW_USEDEFAULT));
    const Math::Vector2i size = descriptor.Size.GetOrDefault(Math::Vector2i(CW_USEDEFAULT));
    // Adjust window rect for client area
    RECT rect = {position.X, position.Y, position.X + size.X, position.Y + size.Y};
    AdjustWindowRectEx(&rect, style, FALSE, exStyle);

    // Convert title to wide string
    // Assume wchar_t is the same size as char16_t
    static_assert(sizeof(wchar_t) == sizeof(char16_t),
                  "wchar_t and char16_t must be the same size in windows platform.");
    const auto* title = reinterpret_cast<const wchar_t*>(descriptor.Title.Data());

    // Create the window
    HWND hwnd = CreateWindowExW(exStyle, L"EdvarWindowClass", title, style, rect.left, rect.top, rect.right - rect.left,
                                rect.bottom - rect.top, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

    if (!hwnd) {
        Platform::Get().OnFatalError(u"Failed to create window");
        // Create a dummy window to return
        hwnd = CreateWindowExW(0, L"STATIC", L"", 0, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);
    }

    // Create WindowsWindow instance
    WindowsWindow* window = new WindowsWindow(hwnd, descriptor);

    // Store in global map
    {
        Threading::ScopedLock lock(g_WindowInstancesMutex);
        g_WindowInstances.Push(window);
    }

    // Store pointer in window user data for message routing
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

    return *window;
}

void WindowsPlatformWindowing::DestroyWindow(IWindowImplementation& window) {
    WindowsWindow* winWindow = static_cast<WindowsWindow*>(&window);

    // Remove from global map
    {
        Threading::ScopedLock lock(g_WindowInstancesMutex);
        for (int32_t i = 0; i < g_WindowInstances.Length(); ++i) {
            if (g_WindowInstances[i] == winWindow) {
                g_WindowInstances.RemoveAt(i);
                break;
            }
        }
    }

    delete winWindow;
}

void WindowsPlatformWindowing::PollEvents() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void WindowsPlatformWindowing::RegisterWindowClass() {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = GlobalWindowProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = L"EdvarWindowClass";

    if (!RegisterClassExW(&wc)) {
        Platform::Get().OnFatalError(u"Failed to register window class");
    }

    WindowClassRegistered = true;
}

void WindowsPlatformWindowing::UpdatePrimaryMonitor() {
    HMONITOR hMonitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    PrimaryMonitor = CreateMonitorInfo(hMonitor);
}

MonitorInfo WindowsPlatformWindowing::CreateMonitorInfo(void* hMonitor) {
    MonitorInfo info;
    info.NativeHandle = hMonitor;

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (GetMonitorInfoW(static_cast<HMONITOR>(hMonitor), &mi)) {
        info.Position = Math::Vector2<int32_t>(mi.rcMonitor.left, mi.rcMonitor.top);
        info.Size =
            Math::Vector2<int32_t>(mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top);
        info.WorkAreaPosition = Math::Vector2<int32_t>(mi.rcWork.left, mi.rcWork.top);
        info.WorkAreaSize = Math::Vector2<int32_t>(mi.rcWork.right - mi.rcWork.left, mi.rcWork.bottom - mi.rcWork.top);
        info.IsPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;

        // Get monitor name
        info.Name = Containers::String(reinterpret_cast<const char16_t*>(mi.szDevice));

        // Get DPI
        UINT dpiX, dpiY;
        if (SUCCEEDED(GetDpiForMonitor(static_cast<HMONITOR>(hMonitor), MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
            info.DPIScale = static_cast<float>(dpiX) / 96.0f;
        }
    }

    return info;
}

// ============================================================================
// WindowsWindow Implementation
// ============================================================================

WindowsWindow::WindowsWindow(void* hwnd, const Windowing::WindowDescriptor& descriptor)
    : NativeHandle(hwnd), CurrentMode(descriptor.Mode), CurrentStyle(descriptor.Style), Visible(descriptor.Visible) {
    UpdateMonitorInfo();
    UpdateDPIScale();
}

WindowsWindow::~WindowsWindow() {
    if (NativeHandle) {
        DestroyWindow(static_cast<HWND>(NativeHandle));
        NativeHandle = nullptr;
    }
}

Containers::String WindowsWindow::GetTitle() const {
    int length = GetWindowTextLengthW(static_cast<HWND>(NativeHandle));
    if (length == 0) {
        return Containers::String();
    }

    wchar_t* buffer = new wchar_t[length + 1];
    GetWindowTextW(static_cast<HWND>(NativeHandle), buffer, length + 1);
    Containers::String title(reinterpret_cast<const char16_t*>(buffer));
    delete[] buffer;
    return title;
}

Math::Vector2<int32_t> WindowsWindow::GetPosition() const {
    RECT rect;
    GetWindowRect(static_cast<HWND>(NativeHandle), &rect);
    return Math::Vector2<int32_t>(rect.left, rect.top);
}

Math::Vector2<int32_t> WindowsWindow::GetSize() const {
    RECT rect;
    GetClientRect(static_cast<HWND>(NativeHandle), &rect);
    return Math::Vector2<int32_t>(rect.right - rect.left, rect.bottom - rect.top);
}

const MonitorInfo& WindowsWindow::GetMonitor() const { return CurrentMonitor; }

float WindowsWindow::GetDPIScale() const { return CurrentDPIScale; }

bool WindowsWindow::IsVisible() const { return Visible; }

bool WindowsWindow::IsFocused() const { return Focused; }

Windowing::WindowMode WindowsWindow::GetMode() const { return CurrentMode; }

Windowing::WindowStyle WindowsWindow::GetStyle() const { return CurrentStyle; }

void WindowsWindow::SetTitle(const Containers::String& title) {
    const wchar_t* wtitle = reinterpret_cast<const wchar_t*>(title.Data());
    SetWindowTextW(static_cast<HWND>(NativeHandle), wtitle);
}

void WindowsWindow::SetPosition(const Math::Vector2<int32_t>& position) {
    SetWindowPos(static_cast<HWND>(NativeHandle), nullptr, position.X, position.Y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void WindowsWindow::SetSize(const Math::Vector2<int32_t>& size) {
    RECT rect = {0, 0, size.X, size.Y};
    DWORD style = GetWindowLongW(static_cast<HWND>(NativeHandle), GWL_STYLE);
    DWORD exStyle = GetWindowLongW(static_cast<HWND>(NativeHandle), GWL_EXSTYLE);
    AdjustWindowRectEx(&rect, style, FALSE, exStyle);

    SetWindowPos(static_cast<HWND>(NativeHandle), nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void WindowsWindow::SetMonitor(const MonitorInfo& monitor) {
    CurrentMonitor = monitor;

    // Move window to monitor
    SetWindowPos(static_cast<HWND>(NativeHandle), nullptr, monitor.Position.X, monitor.Position.Y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    HandleMonitorChanged(CurrentMonitor);
}

void WindowsWindow::SetVisible(bool visible) {
    Visible = visible;
    ShowWindow(static_cast<HWND>(NativeHandle), visible ? SW_SHOW : SW_HIDE);
}

void WindowsWindow::Focus() {
    SetForegroundWindow(static_cast<HWND>(NativeHandle));
    SetFocus(static_cast<HWND>(NativeHandle));
}

void WindowsWindow::SetMode(Windowing::WindowMode mode) {
    CurrentMode = mode;
    ApplyWindowStyle();
}

void WindowsWindow::SetStyle(Windowing::WindowStyle style) {
    CurrentStyle = style;
    ApplyWindowStyle();
}

void WindowsWindow::UpdateMonitorInfo() {
    HMONITOR hMonitor = MonitorFromWindow(static_cast<HWND>(NativeHandle), MONITOR_DEFAULTTOPRIMARY);
    if (hMonitor) {
        MonitorInfo newMonitor = WindowsPlatformWindowing::CreateMonitorInfo(hMonitor);

        // Check if monitor changed
        if (CurrentMonitor.NativeHandle != newMonitor.NativeHandle) {
            CurrentMonitor = newMonitor;
            HandleMonitorChanged(CurrentMonitor);
        }
    }
}

void WindowsWindow::UpdateDPIScale() {
    UINT dpi = GetDpiForWindow(static_cast<HWND>(NativeHandle));
    float newDPI = static_cast<float>(dpi) / 96.0f;

    if (CurrentDPIScale != newDPI) {
        CurrentDPIScale = newDPI;
        HandleDPIChanged(CurrentDPIScale);
    }
}

void WindowsWindow::ApplyWindowStyle() {
    LONG style = 0;
    LONG exStyle = WS_EX_APPWINDOW;

    if (CurrentMode == Windowing::WindowMode::Borderless) {
        style = WS_POPUP;
    } else if (CurrentMode == Windowing::WindowMode::Fullscreen) {
        style = WS_POPUP;
        exStyle |= WS_EX_TOPMOST;
    } else {
        style = WS_OVERLAPPED;
        if (static_cast<uint32_t>(CurrentStyle & Windowing::WindowStyle::Titlebar)) {
            style |= WS_CAPTION;
        }
        if (static_cast<uint32_t>(CurrentStyle & Windowing::WindowStyle::Resizable)) {
            style |= WS_THICKFRAME;
        }
        if (static_cast<uint32_t>(CurrentStyle & Windowing::WindowStyle::Minimizable)) {
            style |= WS_MINIMIZEBOX;
        }
        if (static_cast<uint32_t>(CurrentStyle & Windowing::WindowStyle::Maximizable)) {
            style |= WS_MAXIMIZEBOX;
        }
        if (static_cast<uint32_t>(CurrentStyle & Windowing::WindowStyle::Closable)) {
            style |= WS_SYSMENU;
        }
    }

    if (Visible) {
        style |= WS_VISIBLE;
    }

    SetWindowLongW(static_cast<HWND>(NativeHandle), GWL_STYLE, style);
    SetWindowLongW(static_cast<HWND>(NativeHandle), GWL_EXSTYLE, exStyle);
    SetWindowPos(static_cast<HWND>(NativeHandle), nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void WindowsWindow::HandleDPIChange(uint32_t newDPI, const void* suggestedRect) {
    // Automatically resize window using suggested rect
    const RECT* rect = static_cast<const RECT*>(suggestedRect);
    SetWindowPos(static_cast<HWND>(NativeHandle), nullptr, rect->left, rect->top, rect->right - rect->left,
                 rect->bottom - rect->top, SWP_NOZORDER | SWP_NOACTIVATE);

    // Update DPI scale and fire event
    CurrentDPIScale = static_cast<float>(newDPI) / 96.0f;
    HandleDPIChanged(CurrentDPIScale);
}

void WindowsWindow::HandleMonitorChange() { UpdateMonitorInfo(); }

bool WindowsWindow::OutputSupportsHDR() {
    if (OwnerWrapper) {
        UpdateMonitorInfo();
        return Rendering::IRenderingAPI::GetActiveAPI()->DoesOutputSupportHDR(CurrentMonitor);
    }
    return false;
}
int64_t WindowsWindow::WindowProc(uint32_t msg, uint64_t wParam, int64_t lParam) {
    switch (msg) {
    case WM_CLOSE: {
        HandleClose(0);
        return 0; // Don't destroy automatically
    }

    case WM_SIZE: {
        Math::Vector2<int32_t> newSize(LOWORD(lParam), HIWORD(lParam));
        HandleResized(newSize);
        return 0;
    }

    case WM_MOVE: {
        Math::Vector2<int32_t> newPos(LOWORD(lParam), HIWORD(lParam));
        HandleMoved(newPos);
        return 0;
    }

    case WM_SETFOCUS: {
        Focused = true;
        HandleFocusChanged(true);
        return 0;
    }

    case WM_KILLFOCUS: {
        Focused = false;
        HandleFocusChanged(false);
        return 0;
    }

    case WM_DPICHANGED: {
        uint32_t newDPI = HIWORD(wParam);
        RECT* rect = reinterpret_cast<RECT*>(lParam);
        HandleDPIChange(newDPI, rect);
        return 0;
    }

    case WM_DISPLAYCHANGE: {
        HandleMonitorChange();
        return 0;
    }

    case WM_KEYDOWN:
    case WM_KEYUP: {
        // Route to input system - it will call our HandleKeyEvent
        auto& input = static_cast<Windows::WindowsPlatformInput&>(Platform::Get().GetInput());
        if (WindowsKeyboardDevice* keyboard = input.GetPrimaryKeyboard()) {
            int32_t keyCode = static_cast<int32_t>(wParam);
            bool isDown = (msg == WM_KEYDOWN);
            bool isRepeat = (lParam & 0x40000000) != 0;
            keyboard->ProcessKeyMessage(this, keyCode, isDown, isRepeat);
        }
        return 0;
    }

    case WM_CHAR: {
        // Route to input system - it will call our HandleTextInput
        auto& input = static_cast<Windows::WindowsPlatformInput&>(Platform::Get().GetInput());
        if (WindowsKeyboardDevice* keyboard = input.GetPrimaryKeyboard()) {
            wchar_t ch = static_cast<wchar_t>(wParam);
            Containers::String text(reinterpret_cast<const char16_t*>(&ch), 1);
            keyboard->ProcessTextInput(this, text);
        }
        return 0;
    }

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP: {
        int32_t button = 0;

        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP) {
            button = 0; // Left
        } else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP) {
            button = 1; // Right
        } else {
            button = 2; // Middle
        }

        bool isDown = (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN);

        // Route to input system - it will call our HandleMouseButton
        auto& input = static_cast<Windows::WindowsPlatformInput&>(Platform::Get().GetInput());
        if (WindowsMouseDevice* mouse = input.GetPrimaryMouse()) {
            mouse->ProcessButtonMessage(this, button, isDown);
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        Math::Vector2<int32_t> currentPos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        // Route to input system - it will call our HandleMouseMove
        auto& input = static_cast<Windows::WindowsPlatformInput&>(Platform::Get().GetInput());
        if (WindowsMouseDevice* mouse = input.GetPrimaryMouse()) {
            mouse->ProcessMoveMessage(this, currentPos);
        }

        // Update last mouse position for wheel events
        LastMousePosition = currentPos;
        return 0;
    }

    case WM_MOUSEWHEEL: {
        float wheelDelta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;

        // Route to input system - it will call our HandleMouseWheel
        auto& input = static_cast<Windows::WindowsPlatformInput&>(Platform::Get().GetInput());
        if (WindowsMouseDevice* mouse = input.GetPrimaryMouse()) {
            mouse->ProcessWheelMessage(this, wheelDelta);
        }
        return 0;
    }

    default:
        return DefWindowProcW(static_cast<HWND>(NativeHandle), msg, wParam, lParam);
    }
}

// ============================================================================
// Global Window Procedure
// ============================================================================

static LRESULT CALLBACK GlobalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    if (window) {
        return window->WindowProc(msg, wParam, lParam);
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

} // namespace Edvar::Platform::Windows
