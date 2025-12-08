#include "Platform/IPlatformWindowing.hpp"
#include "Windowing/Window.hpp"
namespace Edvar::Platform {
void IWindowImplementation::HandleClose(int32_t priorityLevel) {
    if (OwnerWrapper) [[likely]] {
        // Call the wrapper's OnClose method.
        OwnerWrapper->OnClose(priorityLevel);
    } else [[unlikely]] {
        // If no wrapper is present, destroy the window immediately.
        Platform::Get().GetWindowing().DestroyWindow(*this);
    }
}

void IWindowImplementation::HandleResized(Math::Vector2<int32_t> newSize) {
    if (OwnerWrapper) [[likely]] {
        OwnerWrapper->HandleResized(newSize);
    }
}

void IWindowImplementation::HandleMoved(Math::Vector2<int32_t> newPosition) {
    if (OwnerWrapper) [[likely]] {
        OwnerWrapper->HandleMoved(newPosition);
    }
}
void IWindowImplementation::HandleFocusChanged(bool focused) {
    if (OwnerWrapper) [[likely]] {
        OwnerWrapper->HandleFocusChanged(focused);
    }
}
void IWindowImplementation::HandleDPIChanged(float newDPI) {
    if (OwnerWrapper) [[likely]] {
        OwnerWrapper->HandleDPIChanged(newDPI);
    }
}
void IWindowImplementation::HandleMonitorChanged(const MonitorInfo& newMonitor) {
    if (OwnerWrapper) [[likely]] {
        OwnerWrapper->HandleMonitorChanged(newMonitor);
    }
}
void IWindowImplementation::HandleDestroyed() {
    if (OwnerWrapper) [[likely]] {
        OwnerWrapper->OnDestroyed();
        OwnerWrapper = nullptr;
    }
}
void IWindowImplementation::HandleKeyEvent(KeyEventArgs& args) {
    if (OwnerWrapper) [[likely]] {
        OwnerWrapper->HandleKeyEvent(args);
    }
}
void IWindowImplementation::HandleMouseButton(MouseButtonEventArgs& args) {
    if (OwnerWrapper) [[likely]] {
        OwnerWrapper->HandleMouseButton(args);
    }
}
void IWindowImplementation::HandleMouseMove(MouseMoveEventArgs& args) {
    if (OwnerWrapper) [[likely]] {
        OwnerWrapper->HandleMouseMove(args);
    }
}
void IWindowImplementation::HandleMouseWheel(MouseWheelEventArgs& args) {
    if (OwnerWrapper) [[likely]] {
        OwnerWrapper->HandleMouseWheel(args);
    }
}
void IWindowImplementation::HandleTextInput(TextInputEventArgs& args) {
    if (OwnerWrapper) [[likely]] {
        OwnerWrapper->HandleTextInput(args);
    }
}
} // namespace Edvar::Platform