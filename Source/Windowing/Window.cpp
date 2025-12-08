#include "Windowing/Window.hpp"
#include "Platform/IPlatform.hpp"
#include "Rendering/ISwapchain.hpp"
#include "Rendering/ResourceDataFormat.hpp"

namespace Edvar::Windowing {

Window::Window(const WindowDescriptor& descriptor) {
    Implementation = &Platform::Get().GetWindowing().CreateWindow(descriptor);

    InitializeRendering();
}

Window::Window() {
    Implementation = &Platform::Get().GetWindowing().CreateWindow(WindowDescriptor());
    InitializeRendering();
}

Window::~Window() {
    if (Implementation) {
        Platform::Get().GetWindowing().DestroyWindow(*Implementation);
        Implementation = nullptr;
    }
    if (Swapchain) {
        Swapchain = nullptr;
    }
}

Window::Window(Window&& other) noexcept : Implementation(other.Implementation) { other.Implementation = nullptr; }

Window& Window::operator=(Window&& other) noexcept {
    if (this != &other) {
        // Destroy current window if any
        if (Implementation) {
            Platform::Get().GetWindowing().DestroyWindow(*Implementation);
        }

        // Take ownership of other's window
        Implementation = other.Implementation;
        other.Implementation = nullptr;
    }
    return *this;
}

// ============================================================================
// Window Properties - Getters
// ============================================================================

Containers::String Window::GetTitle() const {
    return Implementation ? Implementation->GetTitle() : Containers::String();
}

Math::Vector2<int32_t> Window::GetPosition() const {
    return Implementation ? Implementation->GetPosition() : Math::Vector2<int32_t>();
}

Math::Vector2<int32_t> Window::GetSize() const {
    return Implementation ? Implementation->GetSize() : Math::Vector2<int32_t>();
}

const Platform::MonitorInfo& Window::GetMonitor() const {
    static Platform::MonitorInfo dummy;
    return Implementation ? Implementation->GetMonitor() : dummy;
}

float Window::GetDPIScale() const { return Implementation ? Implementation->GetDPIScale() : 1.0f; }

bool Window::IsVisible() const { return Implementation ? Implementation->IsVisible() : false; }

bool Window::IsFocused() const { return Implementation ? Implementation->IsFocused() : false; }

WindowMode Window::GetMode() const { return Implementation ? Implementation->GetMode() : WindowMode::Windowed; }

WindowStyle Window::GetStyle() const { return Implementation ? Implementation->GetStyle() : WindowStyle::Default; }

// ============================================================================
// Window Properties - Setters
// ============================================================================

void Window::SetTitle(const Containers::String& title) {
    if (Implementation) {
        Implementation->SetTitle(title);
    }
}

void Window::SetPosition(const Math::Vector2<int32_t>& position) {
    if (Implementation) {
        Implementation->SetPosition(position);
    }
}

void Window::SetSize(const Math::Vector2<int32_t>& size) {
    if (Implementation) {
        Implementation->SetSize(size);
    }
}

void Window::SetMonitor(const Platform::MonitorInfo& monitor) {
    if (Implementation) {
        Implementation->SetMonitor(monitor);
    }
}

void Window::SetVisible(bool visible) {
    if (Implementation) {
        Implementation->SetVisible(visible);
    }
}

void Window::Focus() {
    if (Implementation) {
        Implementation->Focus();
    }
}

void Window::SetMode(WindowMode mode) {
    if (Implementation) {
        Implementation->SetMode(mode);
    }
}

void Window::SetStyle(WindowStyle style) {
    if (Implementation) {
        Implementation->SetStyle(style);
    }
}

// ============================================================================
// Utility Methods
// ============================================================================

void Window::TryClose(int32_t priorityLevel) {
    if (Implementation) {
        Implementation->HandleClose(priorityLevel);
    } else {
        OnClose(INT32_MAX);
    }
}

void Window::OnClose(int32_t priorityLevel) { Platform::Get().GetWindowing().DestroyWindow(*Implementation); }
void Window::OnDestroyed() { Implementation = nullptr; }
void Window::HandleResized(Math::Vector2<int32_t> newSize) {
    if (Swapchain) {
        Swapchain->SetFullscreen(GetMode() == WindowMode::Fullscreen);
        Swapchain->Resize(newSize, 2, Rendering::ResourceDataFormat::B8G8R8A8_UNorm_SRGB);
    }
}
void Window::InitializeRendering() { Swapchain = Rendering::ISwapchain::CreateForWindow(*this); }
} // namespace Edvar::Windowing
