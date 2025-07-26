#pragma once

#include "Razix/Core/OS/RZKeyCodes.h"
#include "Razix/Core/RZCore.h"

namespace Razix {
    /**
     * Input manager for the Engine to interface with various input devices
     * such as Keyboard, Mouse, Joystick and other HID devices
     * 
     * Implementation is provided by platform-specific files:
     * - GLFWInput.cpp for cross-platform GLFW support
     * - WindowsInput.cpp for native Windows support
     * Build system selects the appropriate implementation per platform
     */
    class RAZIX_API RZInput
    {
    public:
        // Keyboard
        static bool IsKeyPressed(Razix::KeyCode::Key keycode);
        static bool IsKeyReleased(Razix::KeyCode::Key keycode);
        static bool IsKeyHeld(Razix::KeyCode::Key keycode);

        // Mouse
        static bool                IsMouseButtonPressed(Razix::KeyCode::MouseKey button);
        static bool                IsMouseButtonReleased(Razix::KeyCode::MouseKey button);
        static bool                IsMouseButtonHeld(Razix::KeyCode::MouseKey button);
        static std::pair<f32, f32> GetMousePosition();
        static f32                 GetMouseX();
        static f32                 GetMouseY();

        // Gamepad support for multi-device HID (PS5 DualSense, Xbox controllers, etc.)
        static bool IsGamepadConnected();
        static f32  GetJoyLeftStickHorizontal();
        static f32  GetJoyLeftStickVertical();
        static f32  GetJoyRightStickHorizontal();
        static f32  GetJoyRightStickVertical();
        static f32  GetJoyDPadHorizontal();
        static f32  GetJoyDPadVertical();
        static bool IsCrossPressed();
        static bool IsCirclePressed();
        static bool IsTrianglePressed();
        static bool IsSquarePressed();
    };
}    // namespace Razix
