#pragma once

#include "IPlatform.hpp"

namespace Edvar::Windowing {
class Window;
enum class WindowMode : int32_t;
enum class WindowStyle : uint32_t;
struct WindowDescriptor;
} // namespace Edvar::Windowing

namespace Edvar::Platform {

class IWindowImplementation;

// ============================================================================
// Structs
// ============================================================================

struct MonitorInfo {
    void* NativeHandle = nullptr;
    Containers::String Name;
    Math::Vector2<int32_t> Position;
    Math::Vector2<int32_t> Size;
    Math::Vector2<int32_t> WorkAreaPosition;
    Math::Vector2<int32_t> WorkAreaSize;
    float DPIScale = 1.0f;
    bool IsPrimary = false;
};

// ============================================================================
// Event Args
// ============================================================================

struct KeyEventArgs {
    int32_t KeyCode;
    bool IsDown;
    bool IsRepeat;

    void ConsumeInput() { Consumed = true; }
    [[nodiscard]] bool IsConsumed() const { return Consumed; }

private:
    bool Consumed = false;
};

struct MouseButtonEventArgs {
    Math::Vector2<int32_t> Position;
    int32_t Button;
    bool IsDown;

    void ConsumeInput() { Consumed = true; }
    [[nodiscard]] bool IsConsumed() const { return Consumed; }

private:
    bool Consumed = false;
};

struct MouseMoveEventArgs {
    Math::Vector2<int32_t> Position;
    Math::Vector2<int32_t> Delta;

    void ConsumeInput() { Consumed = true; }
    [[nodiscard]] bool IsConsumed() const { return Consumed; }

private:
    bool Consumed = false;
};

struct MouseWheelEventArgs {
    Math::Vector2<int32_t> Position;
    float WheelDelta;

    void ConsumeInput() { Consumed = true; }
    [[nodiscard]] bool IsConsumed() const { return Consumed; }

private:
    bool Consumed = false;
};

struct TextInputEventArgs {
    Containers::String Text;

    void ConsumeInput() { Consumed = true; }
    [[nodiscard]] bool IsConsumed() const { return Consumed; }

private:
    bool Consumed = false;
};

// ============================================================================
// IPlatformWindowing
// ============================================================================

class IPlatformWindowing {
public:
    virtual ~IPlatformWindowing() = default;

    [[nodiscard]] virtual Containers::List<MonitorInfo> GetMonitors() = 0;
    [[nodiscard]] virtual const MonitorInfo& GetPrimaryMonitor() = 0;

    virtual IWindowImplementation& CreateWindow(const Windowing::WindowDescriptor& descriptor) = 0;
    virtual void DestroyWindow(IWindowImplementation& window) = 0;

    virtual void PollEvents() = 0;
};

// ============================================================================
// IWindowImplementation
// ============================================================================

class IWindowImplementation {
public:
    virtual ~IWindowImplementation() = default;

    // Window properties - Getters
    [[nodiscard]] virtual Containers::String GetTitle() const = 0;
    [[nodiscard]] virtual Math::Vector2<int32_t> GetPosition() const = 0;
    [[nodiscard]] virtual Math::Vector2<int32_t> GetSize() const = 0;
    [[nodiscard]] virtual const MonitorInfo& GetMonitor() const = 0;
    [[nodiscard]] virtual float GetDPIScale() const = 0;
    [[nodiscard]] virtual bool IsVisible() const = 0;
    [[nodiscard]] virtual bool IsFocused() const = 0;
    [[nodiscard]] virtual Windowing::WindowMode GetMode() const = 0;
    [[nodiscard]] virtual Windowing::WindowStyle GetStyle() const = 0;

    // Window properties - Setters
    virtual void SetTitle(const Containers::String& title) = 0;
    virtual void SetPosition(const Math::Vector2<int32_t>& position) = 0;
    virtual void SetSize(const Math::Vector2<int32_t>& size) = 0;
    virtual void SetMonitor(const MonitorInfo& monitor) = 0;
    virtual void SetVisible(bool visible) = 0;
    virtual void Focus() = 0;
    virtual void SetMode(Windowing::WindowMode mode) = 0;
    virtual void SetStyle(Windowing::WindowStyle style) = 0;

    // Window lifecycle events - virtual functions that call delegates by default
    virtual void HandleClose(int32_t priorityLevel);
    virtual void HandleDestroyed();
    virtual void HandleResized(Math::Vector2<int32_t> newSize);
    virtual void HandleMoved(Math::Vector2<int32_t> newPosition);
    virtual void HandleFocusChanged(bool focused);
    virtual void HandleDPIChanged(float newDPI);
    virtual void HandleMonitorChanged(const MonitorInfo& newMonitor);

    // Standard input events - virtual functions that call delegates by default
    virtual void HandleKeyEvent(KeyEventArgs& args);
    virtual void HandleMouseButton(MouseButtonEventArgs& args);
    virtual void HandleMouseMove(MouseMoveEventArgs& args);
    virtual void HandleMouseWheel(MouseWheelEventArgs& args);
    virtual void HandleTextInput(TextInputEventArgs& args);

private:
    friend class Edvar::Windowing::Window;
    Edvar::Windowing::Window* OwnerWrapper = nullptr;
};

} // namespace Edvar::Platform
