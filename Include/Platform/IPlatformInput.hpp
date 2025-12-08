#pragma once

namespace Edvar::Platform {

// ============================================================================
// Input Device Types
// ============================================================================

enum class InputDeviceType : int32_t {
    Unknown,
    Keyboard,
    Mouse,
    Gamepad,
    Joystick,
    TouchScreen,
    Pen
};

// ============================================================================
// Input Device Interface
// ============================================================================

class IInputDevice {
public:
    virtual ~IInputDevice() = default;

    [[nodiscard]] virtual InputDeviceType GetDeviceType() const = 0;
    [[nodiscard]] virtual Containers::String GetDeviceName() const = 0;
    [[nodiscard]] virtual void* GetNativeHandle() const = 0;
    [[nodiscard]] virtual bool IsConnected() const = 0;

    // Device events
    Utils::MultiDelegate<void(IInputDevice& device)> OnConnected;
    Utils::MultiDelegate<void(IInputDevice& device)> OnDisconnected;
};

// ============================================================================
// IPlatformInput
// ============================================================================

class IPlatformInput {
public:
    virtual ~IPlatformInput() = default;

    // Device enumeration
    [[nodiscard]] virtual Containers::List<IInputDevice*> GetDevices() = 0;
    [[nodiscard]] virtual Containers::List<IInputDevice*> GetDevicesByType(InputDeviceType type) = 0;

    // Device events
    Utils::MultiDelegate<void(IInputDevice& device)> OnDeviceConnected;
    Utils::MultiDelegate<void(IInputDevice& device)> OnDeviceDisconnected;
};

// ============================================================================
// Keyboard Device Interface
// ============================================================================

class IKeyboardDevice : public IInputDevice {
public:
    struct KeyState {
        int32_t KeyCode;
        bool IsDown;
        bool WasDown;
    };

    // Get current key state
    [[nodiscard]] virtual bool IsKeyDown(int32_t keyCode) const = 0;
    [[nodiscard]] virtual bool WasKeyPressed(int32_t keyCode) const = 0;
    [[nodiscard]] virtual bool WasKeyReleased(int32_t keyCode) const = 0;

    // Keyboard events
    Utils::MultiDelegate<void(IKeyboardDevice& device, int32_t keyCode, bool isDown, bool isRepeat)> OnKey;
    Utils::MultiDelegate<void(IKeyboardDevice& device, const Containers::String& text)> OnTextInput;
};

// ============================================================================
// Mouse Device Interface
// ============================================================================

class IMouseDevice : public IInputDevice {
public:
    struct ButtonState {
        int32_t Button;
        bool IsDown;
        bool WasDown;
    };

    // Get current mouse state
    [[nodiscard]] virtual Math::Vector2<int32_t> GetPosition() const = 0;
    [[nodiscard]] virtual Math::Vector2<int32_t> GetDelta() const = 0;
    [[nodiscard]] virtual bool IsButtonDown(int32_t button) const = 0;
    [[nodiscard]] virtual bool WasButtonPressed(int32_t button) const = 0;
    [[nodiscard]] virtual bool WasButtonReleased(int32_t button) const = 0;

    // Mouse events
    Utils::MultiDelegate<void(IMouseDevice& device, int32_t button, bool isDown)> OnButton;
    Utils::MultiDelegate<void(IMouseDevice& device, Math::Vector2<int32_t> position, Math::Vector2<int32_t> delta)> OnMove;
    Utils::MultiDelegate<void(IMouseDevice& device, float delta)> OnWheel;
};

// ============================================================================
// Gamepad Device Interface
// ============================================================================

class IGamepadDevice : public IInputDevice {
public:
    enum class GamepadType : int32_t {
        Unknown,
        Xbox360,
        XboxOne,
        XboxSeriesX,
        PlayStation4,
        PlayStation5,
        NintendoSwitch,
        Generic
    };

    struct GamepadState {
        // Buttons (bitfield)
        uint32_t Buttons;
        
        // Analog sticks (-1.0 to 1.0)
        Math::Vector2<float> LeftStick;
        Math::Vector2<float> RightStick;
        
        // Triggers (0.0 to 1.0)
        float LeftTrigger;
        float RightTrigger;
    };

    // Common button indices
    enum GamepadButton : uint32_t {
        ButtonA = 1 << 0,
        ButtonB = 1 << 1,
        ButtonX = 1 << 2,
        ButtonY = 1 << 3,
        LeftShoulder = 1 << 4,
        RightShoulder = 1 << 5,
        Back = 1 << 6,
        Start = 1 << 7,
        LeftThumb = 1 << 8,
        RightThumb = 1 << 9,
        DPadUp = 1 << 10,
        DPadDown = 1 << 11,
        DPadLeft = 1 << 12,
        DPadRight = 1 << 13
    };

    // Get controller type
    [[nodiscard]] virtual GamepadType GetGamepadType() const = 0;

    // Get current gamepad state
    [[nodiscard]] virtual GamepadState GetState() const = 0;
    [[nodiscard]] virtual bool IsButtonDown(GamepadButton button) const = 0;
    
    // Haptic feedback
    virtual void SetVibration(float leftMotor, float rightMotor) = 0;

    // Gamepad events
    Utils::MultiDelegate<void(IGamepadDevice& device, GamepadButton button, bool isDown)> OnButton;
    Utils::MultiDelegate<void(IGamepadDevice& device, const GamepadState& state)> OnStateChanged;
};

} // namespace Edvar::Platform
