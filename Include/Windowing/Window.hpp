#pragma once

#include "Platform/IPlatformWindowing.hpp"
namespace Edvar::Rendering {
class ISwapchain;
}
namespace Edvar::Windowing {
enum class WindowMode : int32_t { Windowed, Borderless, Fullscreen };

enum class WindowStyle : uint32_t {
    None = 0,
    Titlebar = 1 << 0,
    Resizable = 1 << 1,
    Minimizable = 1 << 2,
    Maximizable = 1 << 3,
    Closable = 1 << 4,
    Default = Titlebar | Resizable | Minimizable | Maximizable | Closable
};

// Bitwise operators for WindowStyle
constexpr WindowStyle operator|(WindowStyle lhs, WindowStyle rhs) {
    return static_cast<WindowStyle>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr WindowStyle operator&(WindowStyle lhs, WindowStyle rhs) {
    return static_cast<WindowStyle>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr WindowStyle operator~(WindowStyle style) { return static_cast<WindowStyle>(~static_cast<uint32_t>(style)); }

inline WindowStyle& operator|=(WindowStyle& lhs, WindowStyle rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline WindowStyle& operator&=(WindowStyle& lhs, WindowStyle rhs) {
    lhs = lhs & rhs;
    return lhs;
}

struct WindowDescriptor {
    Containers::String Title = u"Edvar Window";
    Utils::Optional<Math::Vector2i> Position;
    Utils::Optional<Math::Vector2i> Size;
    WindowMode Mode = WindowMode::Windowed;
    WindowStyle Style = WindowStyle::Default;
    bool Visible = true;
    Utils::Optional<Platform::MonitorInfo> TargetMonitor;
};

/**
 * @brief High-level wrapper class for platform window management
 *
 * This class provides a convenient RAII wrapper around the platform's windowing system.
 * It automatically manages the lifetime of the underlying platform window and provides
 * a clean interface for window operations.
 */
class EDVAR_CPP_CORE_API Window : public Memory::EnableSharedFromThis<Window> {

    /**
     * @brief Creates a new window with the specified descriptor
     * @param descriptor Configuration for the window (title, size, position, etc.)
     */
    explicit Window(const WindowDescriptor& descriptor);

    /**
     * @brief Creates a window with default settings
     */
    Window();

public:
    static SharedReference<Window> Create(const WindowDescriptor& descriptor) { return {new Window(descriptor)}; }

    /**
     * @brief Destructor - automatically destroys the platform window
     */
    ~Window() override;

    // Disable copy
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // Enable move
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    // ========================================================================
    // Window Properties - Getters
    // ========================================================================

    [[nodiscard]] Containers::String GetTitle() const;
    [[nodiscard]] Math::Vector2<int32_t> GetPosition() const;
    [[nodiscard]] Math::Vector2<int32_t> GetSize() const;
    [[nodiscard]] const Platform::MonitorInfo& GetMonitor() const;
    [[nodiscard]] float GetDPIScale() const;
    [[nodiscard]] bool IsVisible() const;
    [[nodiscard]] bool IsFocused() const;
    [[nodiscard]] WindowMode GetMode() const;
    [[nodiscard]] WindowStyle GetStyle() const;
    [[nodiscard]] bool IsUsingHDRWhenPossible() const { return useHDRWhenPossible; }
    [[nodiscard]] bool IsHDREnabled() const { return isHDREnabled; }

    // ========================================================================
    // Window Properties - Setters
    // ========================================================================

    void SetTitle(const Containers::String& title);
    void SetPosition(const Math::Vector2<int32_t>& position);
    void SetSize(const Math::Vector2<int32_t>& size);
    void SetMonitor(const Platform::MonitorInfo& monitor);
    void SetVisible(bool visible);
    void Focus();
    void SetMode(WindowMode mode);
    void SetStyle(WindowStyle style);
    void SetUseHDRWhenPossible(bool useHDR);

    /**
     * @brief Called when the window has requested to close.
     * Either with ALT+F4 or the close button.
     */
    virtual void OnClose(int32_t priorityLevel);
    virtual void OnDestroyed();

    /**
     * @brief Called when the window is resized
     * @param newSize The new window size
     */
    virtual void HandleResized(Math::Vector2<int32_t> newSize);

    /**
     * @brief Called when the window is moved
     * @param newPosition The new window position
     */
    virtual void HandleMoved(Math::Vector2<int32_t> newPosition);

    /**
     * @brief Called when focus changes
     * @param focused True if window gained focus, false if lost
     */
    virtual void HandleFocusChanged(bool focused);

    /**
     * @brief Called when DPI scale changes
     * @param newDPI The new DPI scale
     */
    virtual void HandleDPIChanged(float newDPI);

    /**
     * @brief Called when monitor changes
     * @param newMonitor The new monitor
     */
    virtual void HandleMonitorChanged(const Platform::MonitorInfo& newMonitor);

    /**
     * @brief Called when a key event occurs
     * @param args Key event arguments
     */
    virtual void HandleKeyEvent(Platform::KeyEventArgs& args);

    /**
     * @brief Called when a mouse button event occurs
     * @param args Mouse button event arguments
     */
    virtual void HandleMouseButton(Platform::MouseButtonEventArgs& args);

    /**
     * @brief Called when mouse moves
     * @param args Mouse move event arguments
     */
    virtual void HandleMouseMove(Platform::MouseMoveEventArgs& args);

    /**
     * @brief Called when mouse wheel scrolls
     * @param args Mouse wheel event arguments
     */
    virtual void HandleMouseWheel(Platform::MouseWheelEventArgs& args);

    /**
     * @brief Called when text input occurs
     * @param args Text input event arguments
     */
    virtual void HandleTextInput(Platform::TextInputEventArgs& args);

    // ========================================================================
    // Utility Methods
    // ========================================================================

    /**
     * @brief Check if the window is valid
     * @return True if the window has a valid platform implementation
     */
    [[nodiscard]] bool IsValid() const { return implementation != nullptr; }

    [[nodiscard]] bool DoesCurrentOutputSupportHDR() const;

    /**
     * @brief Get the underlying platform implementation
     * @return Reference to the platform window implementation
     */
    [[nodiscard]] Platform::IWindowImplementation& GetImplementation() const { return *implementation; }
    void TryClose(int32_t priorityLevel = 0);

    SharedPointer<Rendering::ISwapchain> GetSwapchain() const { return swapchain; }

private:
    Platform::IWindowImplementation* implementation = nullptr;
    bool useHDRWhenPossible = true;
    bool isHDREnabled = false;
    float currentDPI = 96.0f;
    SharedPointer<Rendering::ISwapchain> swapchain;

    void InitializeRendering();
};

} // namespace Edvar::Windowing
