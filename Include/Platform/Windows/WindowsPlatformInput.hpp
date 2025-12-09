#pragma once

#include "Platform/IPlatformInput.hpp"
#include "Platform/IPlatformWindowing.hpp"

namespace Edvar::Platform::Windows {

class WindowsKeyboardDevice;
class WindowsMouseDevice;
class WindowsGamepadDevice;

class WindowsPlatformInput final : public IPlatformInput {
public:
    WindowsPlatformInput();
    ~WindowsPlatformInput() override;

    [[nodiscard]] Containers::List<IInputDevice*> GetDevices() override;
    [[nodiscard]] Containers::List<IInputDevice*> GetDevicesByType(InputDeviceType type) override;

    void Update();

    // Internal methods for routing Windows messages
    [[nodiscard]] WindowsKeyboardDevice* GetPrimaryKeyboard() const { return PrimaryKeyboard; }
    [[nodiscard]] WindowsMouseDevice* GetPrimaryMouse() const { return PrimaryMouse; }

    IInputDevice* GetKeyboard() override;
    IInputDevice* GetMouse() override;

private:
    Containers::List<IInputDevice*> Devices;
    WindowsKeyboardDevice* PrimaryKeyboard = nullptr;
    WindowsMouseDevice* PrimaryMouse = nullptr;

    void EnumerateDevices();
    void EnumerateGamepads();
};

class WindowsKeyboardDevice final : public IKeyboardDevice {
public:
    explicit WindowsKeyboardDevice();
    ~WindowsKeyboardDevice() override;

    [[nodiscard]] InputDeviceType GetDeviceType() const override { return InputDeviceType::Keyboard; }
    [[nodiscard]] Containers::String GetDeviceName() const override { return DeviceName; }
    [[nodiscard]] void* GetNativeHandle() const override { return nullptr; }
    [[nodiscard]] bool IsConnected() const override { return Connected; }

    [[nodiscard]] bool IsKeyDown(int32_t keyCode) const override;
    [[nodiscard]] bool WasKeyPressed(int32_t keyCode) const override;
    [[nodiscard]] bool WasKeyReleased(int32_t keyCode) const override;

    void ProcessKeyMessage(IWindowImplementation* window, int32_t keyCode, bool isDown, bool isRepeat);
    void ProcessTextInput(IWindowImplementation* window, const Containers::String& text);
    void UpdateState();

private:
    Containers::String DeviceName;
    bool Connected = true;

    static constexpr int32_t MaxKeys = 256;
    bool CurrentKeyState[MaxKeys] = {};
    bool PreviousKeyState[MaxKeys] = {};
};

class WindowsMouseDevice final : public IMouseDevice {
public:
    WindowsMouseDevice();
    ~WindowsMouseDevice() override;

    [[nodiscard]] InputDeviceType GetDeviceType() const override { return InputDeviceType::Mouse; }
    [[nodiscard]] Containers::String GetDeviceName() const override { return DeviceName; }
    [[nodiscard]] void* GetNativeHandle() const override { return nullptr; }
    [[nodiscard]] bool IsConnected() const override { return Connected; }

    [[nodiscard]] Math::Vector2<int32_t> GetPosition() const override { return Position; }
    [[nodiscard]] Math::Vector2<int32_t> GetDelta() const override { return Delta; }
    [[nodiscard]] bool IsButtonDown(int32_t button) const override;
    [[nodiscard]] bool WasButtonPressed(int32_t button) const override;
    [[nodiscard]] bool WasButtonReleased(int32_t button) const override;

    void ProcessButtonMessage(IWindowImplementation* window, int32_t button, bool isDown);
    void ProcessMoveMessage(IWindowImplementation* window, const Math::Vector2<int32_t>& position);
    void ProcessWheelMessage(IWindowImplementation* window, float delta);
    void UpdateState();

private:
    Containers::String DeviceName;
    bool Connected = true;

    Math::Vector2<int32_t> Position;
    Math::Vector2<int32_t> Delta;

    static constexpr int32_t MaxButtons = 5;
    bool CurrentButtonState[MaxButtons] = {};
    bool PreviousButtonState[MaxButtons] = {};
};

class WindowsGamepadDevice final : public IGamepadDevice {
public:
    explicit WindowsGamepadDevice(int32_t xinputIndex);
    ~WindowsGamepadDevice() override;

    [[nodiscard]] InputDeviceType GetDeviceType() const override { return InputDeviceType::Gamepad; }
    [[nodiscard]] Containers::String GetDeviceName() const override { return DeviceName; }
    [[nodiscard]] void* GetNativeHandle() const override {
        return reinterpret_cast<void*>(static_cast<intptr_t>(XInputIndex));
    }
    [[nodiscard]] bool IsConnected() const override { return Connected; }

    [[nodiscard]] GamepadType GetGamepadType() const override { return ControllerType; }
    [[nodiscard]] GamepadState GetState() const override { return CurrentState; }
    [[nodiscard]] bool IsButtonDown(GamepadButton button) const override;

    void SetVibration(float leftMotor, float rightMotor) override;
    void UpdateState();

private:
    Containers::String DeviceName;
    int32_t XInputIndex;
    GamepadType ControllerType = GamepadType::Unknown;
    bool Connected = false;

    GamepadState CurrentState = {};
    GamepadState PreviousState = {};

    void DetectControllerType();
};

} // namespace Edvar::Platform::Windows
