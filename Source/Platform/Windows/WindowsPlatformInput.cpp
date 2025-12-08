#include "ForceInclude.hpp"
#include "Platform/Windows/WindowsPlatformInput.hpp"
#include "Platform/IPlatform.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>

#pragma comment(lib, "Xinput.lib")

namespace Edvar::Platform::Windows {

// ============================================================================
// WindowsKeyboardDevice
// ============================================================================

WindowsKeyboardDevice::WindowsKeyboardDevice(const Containers::String& name)
    : DeviceName(name) {
    for (int32_t i = 0; i < MaxKeys; ++i) {
        CurrentKeyState[i] = false;
        PreviousKeyState[i] = false;
    }
}

WindowsKeyboardDevice::~WindowsKeyboardDevice() {}

bool WindowsKeyboardDevice::IsKeyDown(int32_t keyCode) const {
    if (keyCode < 0 || keyCode >= MaxKeys) {
        return false;
    }
    return CurrentKeyState[keyCode];
}

bool WindowsKeyboardDevice::WasKeyPressed(int32_t keyCode) const {
    if (keyCode < 0 || keyCode >= MaxKeys) {
        return false;
    }
    return CurrentKeyState[keyCode] && !PreviousKeyState[keyCode];
}

bool WindowsKeyboardDevice::WasKeyReleased(int32_t keyCode) const {
    if (keyCode < 0 || keyCode >= MaxKeys) {
        return false;
    }
    return !CurrentKeyState[keyCode] && PreviousKeyState[keyCode];
}

void WindowsKeyboardDevice::ProcessKeyMessage(IWindowImplementation* window, int32_t keyCode, bool isDown, bool isRepeat) {
    if (keyCode >= 0 && keyCode < MaxKeys) {
        bool wasDown = CurrentKeyState[keyCode];
        CurrentKeyState[keyCode] = isDown;
        
        // Fire device event
        OnKey.Broadcast(*this, keyCode, isDown, isRepeat);
        
        // Call window event handler if window provided
        if (window) {
            KeyEventArgs args;
            args.KeyCode = keyCode;
            args.IsDown = isDown;
            args.IsRepeat = isRepeat;
            window->HandleKeyEvent(args);
        }
    }
}

void WindowsKeyboardDevice::ProcessTextInput(IWindowImplementation* window, const Containers::String& text) {
    // Fire device event
    OnTextInput.Broadcast(*this, text);
    
    // Call window event handler if window provided
    if (window) {
        TextInputEventArgs args;
        args.Text = text;
        window->HandleTextInput(args);
    }
}

void WindowsKeyboardDevice::UpdateState() {
    // Copy current state to previous for next frame
    for (int32_t i = 0; i < MaxKeys; ++i) {
        PreviousKeyState[i] = CurrentKeyState[i];
    }
}

// ============================================================================
// WindowsMouseDevice
// ============================================================================

WindowsMouseDevice::WindowsMouseDevice(const Containers::String& name)
    : DeviceName(name) {
    for (int32_t i = 0; i < MaxButtons; ++i) {
        CurrentButtonState[i] = false;
        PreviousButtonState[i] = false;
    }
    Position = Math::Vector2<int32_t>(0, 0);
    Delta = Math::Vector2<int32_t>(0, 0);
}

WindowsMouseDevice::~WindowsMouseDevice() {}

bool WindowsMouseDevice::IsButtonDown(int32_t button) const {
    if (button < 0 || button >= MaxButtons) {
        return false;
    }
    return CurrentButtonState[button];
}

bool WindowsMouseDevice::WasButtonPressed(int32_t button) const {
    if (button < 0 || button >= MaxButtons) {
        return false;
    }
    return CurrentButtonState[button] && !PreviousButtonState[button];
}

bool WindowsMouseDevice::WasButtonReleased(int32_t button) const {
    if (button < 0 || button >= MaxButtons) {
        return false;
    }
    return !CurrentButtonState[button] && PreviousButtonState[button];
}

void WindowsMouseDevice::ProcessButtonMessage(IWindowImplementation* window, int32_t button, bool isDown) {
    if (button >= 0 && button < MaxButtons) {
        bool wasDown = CurrentButtonState[button];
        CurrentButtonState[button] = isDown;
        
        // Fire device event
        OnButton.Broadcast(*this, button, isDown);
        
        // Call window event handler if window provided
        if (window) {
            MouseButtonEventArgs args;
            args.Position = Position;
            args.Button = button;
            args.IsDown = isDown;
            window->HandleMouseButton(args);
        }
    }
}

void WindowsMouseDevice::ProcessMoveMessage(IWindowImplementation* window, const Math::Vector2<int32_t>& position) {
    Delta.X = position.X - Position.X;
    Delta.Y = position.Y - Position.Y;
    Position = position;
    
    // Fire device event
    OnMove.Broadcast(*this, Position, Delta);
    
    // Call window event handler if window provided
    if (window) {
        MouseMoveEventArgs args;
        args.Position = Position;
        args.Delta = Delta;
        window->HandleMouseMove(args);
    }
}

void WindowsMouseDevice::ProcessWheelMessage(IWindowImplementation* window, float delta) {
    // Fire device event
    OnWheel.Broadcast(*this, delta);
    
    // Call window event handler if window provided
    if (window) {
        MouseWheelEventArgs args;
        args.Position = Position;
        args.WheelDelta = delta;
        window->HandleMouseWheel(args);
    }
}

void WindowsMouseDevice::UpdateState() {
    // Copy current state to previous for next frame
    for (int32_t i = 0; i < MaxButtons; ++i) {
        PreviousButtonState[i] = CurrentButtonState[i];
    }
    // Reset delta
    Delta = Math::Vector2<int32_t>(0, 0);
}

// ============================================================================
// WindowsGamepadDevice
// ============================================================================

WindowsGamepadDevice::WindowsGamepadDevice(int32_t xinputIndex)
    : XInputIndex(xinputIndex) {
    CurrentState.Buttons = 0;
    CurrentState.LeftStick = Math::Vector2<float>(0.0f, 0.0f);
    CurrentState.RightStick = Math::Vector2<float>(0.0f, 0.0f);
    CurrentState.LeftTrigger = 0.0f;
    CurrentState.RightTrigger = 0.0f;
    
    // Try to detect controller type
    DetectControllerType();
    
    // Update to check if connected
    UpdateState();
}

WindowsGamepadDevice::~WindowsGamepadDevice() {}

bool WindowsGamepadDevice::IsButtonDown(GamepadButton button) const {
    return (CurrentState.Buttons & static_cast<uint32_t>(button)) != 0;
}

void WindowsGamepadDevice::SetVibration(float leftMotor, float rightMotor) {
    if (!Connected) {
        return;
    }
    
    XINPUT_VIBRATION vibration;
    vibration.wLeftMotorSpeed = static_cast<WORD>(leftMotor * 65535.0f);
    vibration.wRightMotorSpeed = static_cast<WORD>(rightMotor * 65535.0f);
    XInputSetState(XInputIndex, &vibration);
}

void WindowsGamepadDevice::UpdateState() {
    // Copy current state to previous for next frame
    PreviousState = CurrentState;
    
    // Poll XInput state
    XINPUT_STATE xinputState;
    DWORD result = XInputGetState(XInputIndex, &xinputState);
    
    if (result == ERROR_SUCCESS) {
        if (!Connected) {
            Connected = true;
            OnConnected.Broadcast(*this);
        }
        
        // Map XInput buttons to our button bitfield
        CurrentState.Buttons = 0;
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::ButtonA);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_B) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::ButtonB);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_X) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::ButtonX);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::ButtonY);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::LeftShoulder);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::RightShoulder);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::Back);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_START) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::Start);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::LeftThumb);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::RightThumb);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::DPadUp);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::DPadDown);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::DPadLeft);
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) CurrentState.Buttons |= static_cast<uint32_t>(GamepadButton::DPadRight);
        
        // Apply deadzone and normalize sticks
        auto ApplyDeadzone = [](SHORT value, SHORT deadzone) -> float {
            if (value < -deadzone) {
                return static_cast<float>(value + deadzone) / (32768.0f - static_cast<float>(deadzone));
            } else if (value > deadzone) {
                return static_cast<float>(value - deadzone) / (32767.0f - static_cast<float>(deadzone));
            }
            return 0.0f;
        };
        
        CurrentState.LeftStick.X = ApplyDeadzone(xinputState.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        CurrentState.LeftStick.Y = ApplyDeadzone(xinputState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        CurrentState.RightStick.X = ApplyDeadzone(xinputState.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        CurrentState.RightStick.Y = ApplyDeadzone(xinputState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        
        // Normalize triggers
        CurrentState.LeftTrigger = xinputState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD 
            ? static_cast<float>(xinputState.Gamepad.bLeftTrigger - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / (255.0f - static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
            : 0.0f;
        CurrentState.RightTrigger = xinputState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD
            ? static_cast<float>(xinputState.Gamepad.bRightTrigger - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / (255.0f - static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
            : 0.0f;
        
        // Fire state changed event
        OnStateChanged.Broadcast(*this, CurrentState);
    } else {
        if (Connected) {
            Connected = false;
            OnDisconnected.Broadcast(*this);
        }
    }
}

void WindowsGamepadDevice::DetectControllerType() {
    // Try to detect controller type using XInput capabilities
    XINPUT_CAPABILITIES caps;
    if (XInputGetCapabilities(XInputIndex, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS) {
        // XInput supports Xbox controllers primarily
        // SubType can give us hints about the controller
        switch (caps.SubType) {
        case XINPUT_DEVSUBTYPE_GAMEPAD:
            // Generic XInput controller - could be Xbox 360, One, or Series
            // We'll default to XboxOne as it's most common
            ControllerType = GamepadType::XboxOne;
            DeviceName = u"Xbox Controller";
            break;
        case XINPUT_DEVSUBTYPE_WHEEL:
            ControllerType = GamepadType::Generic;
            DeviceName = u"Racing Wheel";
            break;
        case XINPUT_DEVSUBTYPE_ARCADE_STICK:
            ControllerType = GamepadType::Generic;
            DeviceName = u"Arcade Stick";
            break;
        case XINPUT_DEVSUBTYPE_FLIGHT_STICK:
            ControllerType = GamepadType::Generic;
            DeviceName = u"Flight Stick";
            break;
        case XINPUT_DEVSUBTYPE_DANCE_PAD:
            ControllerType = GamepadType::Generic;
            DeviceName = u"Dance Pad";
            break;
        case XINPUT_DEVSUBTYPE_GUITAR:
        case XINPUT_DEVSUBTYPE_GUITAR_ALTERNATE:
        case XINPUT_DEVSUBTYPE_GUITAR_BASS:
            ControllerType = GamepadType::Generic;
            DeviceName = u"Guitar Controller";
            break;
        case XINPUT_DEVSUBTYPE_DRUM_KIT:
            ControllerType = GamepadType::Generic;
            DeviceName = u"Drum Kit";
            break;
        case XINPUT_DEVSUBTYPE_ARCADE_PAD:
            ControllerType = GamepadType::Generic;
            DeviceName = u"Arcade Pad";
            break;
        default:
            ControllerType = GamepadType::XboxOne;
            DeviceName = u"Xbox Controller";
            break;
        }
    } else {
        ControllerType = GamepadType::Unknown;
        DeviceName = u"Disconnected Gamepad";
    }
}

// ============================================================================
// WindowsPlatformInput
// ============================================================================

WindowsPlatformInput::WindowsPlatformInput() {
    // Create primary keyboard and mouse devices
    EnumerateDevices();
    
    // Enumerate XInput gamepads
    EnumerateGamepads();
}

WindowsPlatformInput::~WindowsPlatformInput() {
    // Clean up device instances
    for (int32_t i = 0; i < Devices.Length(); ++i) {
        delete Devices[i];
    }
}

Containers::List<IInputDevice*> WindowsPlatformInput::GetDevices() {
    return Devices;
}

Containers::List<IInputDevice*> WindowsPlatformInput::GetDevicesByType(InputDeviceType type) {
    Containers::List<IInputDevice*> result;
    for (int32_t i = 0; i < Devices.Length(); ++i) {
        if (Devices[i]->GetDeviceType() == type) {
            result.Push(Devices[i]);
        }
    }
    return result;
}

void WindowsPlatformInput::EnumerateDevices() {
    // Create primary keyboard device
    WindowsKeyboardDevice* keyboard = new WindowsKeyboardDevice(u"Primary Keyboard");
    Devices.Push(keyboard);
    PrimaryKeyboard = keyboard;
    
    // Create primary mouse device
    WindowsMouseDevice* mouse = new WindowsMouseDevice(u"Primary Mouse");
    Devices.Push(mouse);
    PrimaryMouse = mouse;
}

void WindowsPlatformInput::EnumerateGamepads() {
    // XInput supports up to 4 controllers
    for (int32_t i = 0; i < XUSER_MAX_COUNT; ++i) {
        WindowsGamepadDevice* gamepad = new WindowsGamepadDevice(i);
        Devices.Push(gamepad);
    }
}

void WindowsPlatformInput::Update() {
    // Update all devices
    for (int32_t i = 0; i < Devices.Length(); ++i) {
        if (Devices[i]->GetDeviceType() == InputDeviceType::Keyboard) {
            static_cast<WindowsKeyboardDevice*>(Devices[i])->UpdateState();
        } else if (Devices[i]->GetDeviceType() == InputDeviceType::Mouse) {
            static_cast<WindowsMouseDevice*>(Devices[i])->UpdateState();
        } else if (Devices[i]->GetDeviceType() == InputDeviceType::Gamepad) {
            static_cast<WindowsGamepadDevice*>(Devices[i])->UpdateState();
        }
    }
}

} // namespace Edvar::Platform::Windows
