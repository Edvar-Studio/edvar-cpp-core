#pragma once

#include "Platform/IPlatformWindowing.hpp"

namespace Edvar::Platform::Windows {

class WindowsWindow;

class WindowsPlatformWindowing final : public IPlatformWindowing {
public:
    WindowsPlatformWindowing();
    ~WindowsPlatformWindowing() override;

    [[nodiscard]] Containers::List<MonitorInfo> GetMonitors() override;
    [[nodiscard]] const MonitorInfo& GetPrimaryMonitor() override;

    IWindowImplementation& CreateWindow(const Windowing::WindowDescriptor& descriptor) override;
    void DestroyWindow(IWindowImplementation& window) override;

    void PollEvents() override;

private:
    MonitorInfo PrimaryMonitor;
    bool WindowClassRegistered = false;

    void RegisterWindowClass();
    void UpdatePrimaryMonitor();
    static MonitorInfo CreateMonitorInfo(void* hMonitor);
    friend class WindowsWindow;
};

class WindowsWindow final : public IWindowImplementation {
public:
    explicit WindowsWindow(void* hwnd, const Windowing::WindowDescriptor& descriptor);
    ~WindowsWindow() override;

    // Window properties - Getters
    [[nodiscard]] Containers::String GetTitle() const override;
    [[nodiscard]] Math::Vector2<int32_t> GetPosition() const override;
    [[nodiscard]] Math::Vector2<int32_t> GetSize() const override;
    [[nodiscard]] const MonitorInfo& GetMonitor() const override;
    [[nodiscard]] float GetDPIScale() const override;
    [[nodiscard]] bool IsVisible() const override;
    [[nodiscard]] bool IsFocused() const override;
    [[nodiscard]] Windowing::WindowMode GetMode() const override;
    [[nodiscard]] Windowing::WindowStyle GetStyle() const override;

    // Window properties - Setters
    void SetTitle(const Containers::String& title) override;
    void SetPosition(const Math::Vector2<int32_t>& position) override;
    void SetSize(const Math::Vector2<int32_t>& size) override;
    void SetMonitor(const MonitorInfo& monitor) override;
    void SetVisible(bool visible) override;
    void Focus() override;
    void SetMode(Windowing::WindowMode mode) override;
    void SetStyle(Windowing::WindowStyle style) override;

    [[nodiscard]] void* GetNativeHandle() const { return NativeHandle; }

    // Window procedure for message routing
    int64_t WindowProc(uint32_t msg, uint64_t wParam, int64_t lParam);

private:
    void* NativeHandle; // HWND
    MonitorInfo CurrentMonitor;
    Windowing::WindowMode CurrentMode;
    Windowing::WindowStyle CurrentStyle;
    float CurrentDPIScale = 1.0f;
    bool Visible = true;
    bool Focused = false;
    Math::Vector2<int32_t> LastMousePosition;

    void UpdateMonitorInfo();
    void UpdateDPIScale();
    void ApplyWindowStyle();
    void HandleDPIChange(uint32_t newDPI, const void* suggestedRect);
    void HandleMonitorChange();
};

} // namespace Edvar::Platform::Windows
