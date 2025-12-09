#include "Windowing/Window.hpp"
#include "Platform/IPlatform.hpp"
#include "Rendering/IRenderDevice.hpp"
#include "Rendering/ISwapchain.hpp"
#include "Rendering/ResourceDataFormat.hpp"

namespace Edvar::Windowing {

Window::Window(const WindowDescriptor& descriptor) {
    Platform::Get().PrintMessageToDebugger(*String::PrintF(u"Sending request to create window: %s", *descriptor.Title));
    implementation = &Platform::Get().GetWindowing().CreateWindow(descriptor);
    Platform::Get().PrintMessageToDebugger(
        *String::PrintF(u"Window created with implementation handle: %p", implementation));
    Platform::Get().PrintMessageToDebugger(
        *String::PrintF(u"Now initializing rendering for window: %s", *descriptor.Title));
    InitializeRendering();
}

Window::Window() {
    implementation = &Platform::Get().GetWindowing().CreateWindow(WindowDescriptor());
    InitializeRendering();
}

Window::~Window() {
    if (implementation) {
        Platform::Get().GetWindowing().DestroyWindow(*implementation);
        implementation = nullptr;
    }
    if (swapchain) {
        swapchain = nullptr;
    }
}

Window::Window(Window&& other) noexcept : implementation(other.implementation) { other.implementation = nullptr; }

Window& Window::operator=(Window&& other) noexcept {
    if (this != &other) {
        // Destroy current window if any
        if (implementation) {
            Platform::Get().GetWindowing().DestroyWindow(*implementation);
        }

        // Take ownership of other's window
        implementation = other.implementation;
        other.implementation = nullptr;
    }
    return *this;
}

// ============================================================================
// Window Properties - Getters
// ============================================================================

Containers::String Window::GetTitle() const {
    return implementation ? implementation->GetTitle() : Containers::String();
}

Math::Vector2<int32_t> Window::GetPosition() const {
    return implementation ? implementation->GetPosition() : Math::Vector2<int32_t>();
}

Math::Vector2<int32_t> Window::GetSize() const {
    return implementation ? implementation->GetSize() : Math::Vector2<int32_t>();
}

const Platform::MonitorInfo& Window::GetMonitor() const {
    static Platform::MonitorInfo dummy;
    return implementation ? implementation->GetMonitor() : dummy;
}

float Window::GetDPIScale() const { return implementation ? implementation->GetDPIScale() : 1.0f; }

bool Window::IsVisible() const { return implementation ? implementation->IsVisible() : false; }

bool Window::IsFocused() const { return implementation ? implementation->IsFocused() : false; }

WindowMode Window::GetMode() const { return implementation ? implementation->GetMode() : WindowMode::Windowed; }

WindowStyle Window::GetStyle() const { return implementation ? implementation->GetStyle() : WindowStyle::Default; }

// ============================================================================
// Window Properties - Setters
// ============================================================================

void Window::SetTitle(const Containers::String& title) {
    if (implementation) {
        implementation->SetTitle(title);
    }
}

void Window::SetPosition(const Math::Vector2<int32_t>& position) {
    if (implementation) {
        implementation->SetPosition(position);
    }
}

void Window::SetSize(const Math::Vector2<int32_t>& size) {
    if (implementation) {
        implementation->SetSize(size);
    }
}

void Window::SetMonitor(const Platform::MonitorInfo& monitor) {
    if (implementation) {
        implementation->SetMonitor(monitor);
    }
}

void Window::SetVisible(bool visible) {
    if (implementation) {
        implementation->SetVisible(visible);
    }
}

void Window::Focus() {
    if (implementation) {
        implementation->Focus();
    }
}

void Window::SetMode(WindowMode mode) {
    if (implementation) {
        implementation->SetMode(mode);
    }
}

void Window::SetStyle(WindowStyle style) {
    if (implementation) {
        implementation->SetStyle(style);
    }
}
void Window::SetUseHDRWhenPossible(bool useHDR) {
    useHDRWhenPossible = useHDR;
    GetSwapchain()->SetHDRMode(useHDR);
}

// ============================================================================
// Utility Methods
// ============================================================================

bool Window::DoesCurrentOutputSupportHDR() const {
    if (implementation) {
        return implementation->OutputSupportsHDR();
    }
    return false;
}
void Window::TryClose(int32_t priorityLevel) {
    if (implementation) {
        implementation->HandleClose(priorityLevel);
    } else {
        OnClose(INT32_MAX);
    }
}

void Window::OnClose(int32_t priorityLevel) { Platform::Get().GetWindowing().DestroyWindow(*implementation); }
void Window::OnDestroyed() { implementation = nullptr; }
void Window::HandleResized(Math::Vector2<int32_t> newSize) {
    if (swapchain) {
        swapchain->SetFullscreen(GetMode() == WindowMode::Fullscreen, newSize);
        swapchain->Resize(newSize, 2, Rendering::ResourceDataFormat::B8G8R8A8_UNorm_SRGB);
    }
}
void Window::HandleMoved(Math::Vector2<int32_t> newPosition) {}
void Window::HandleFocusChanged(bool focused) {}
void Window::HandleDPIChanged(float newDPI) {
    currentDPI = newDPI;
    if (GetSwapchain()) {
        GetSwapchain()->SetHDRMode(useHDRWhenPossible);
    }
}
void Window::HandleMonitorChanged(const Platform::MonitorInfo& newMonitor) {
    if (GetSwapchain()) {
        GetSwapchain()->SetHDRMode(useHDRWhenPossible);
    }
}
void Window::HandleKeyEvent(Platform::KeyEventArgs& args) {}
void Window::HandleMouseButton(Platform::MouseButtonEventArgs& args) {}
void Window::HandleMouseMove(Platform::MouseMoveEventArgs& args) {}
void Window::HandleMouseWheel(Platform::MouseWheelEventArgs& args) {}
void Window::HandleTextInput(Platform::TextInputEventArgs& args) {}
void Window::InitializeRendering() {
    swapchain = Rendering::IRenderingAPI::GetActiveAPI()->GetPrimaryDevice()->CreateSwapchain(
        *this, Rendering::ResourceDataFormat::B8G8R8A8_UNorm_SRGB);
    if (IsUsingHDRWhenPossible()) {
        GetSwapchain()->SetHDRMode(useHDRWhenPossible);
    }
}
} // namespace Edvar::Windowing
