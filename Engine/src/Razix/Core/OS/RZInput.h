#pragma once

#include "Razix/Core/OS/RZKeyCodes.h"
#include "Razix/Core/RZCore.h"

namespace Razix {
    /**
     * Input manager for the Engine to interface with various input devices
     * such as Keyboard, Mouse, Joystick and other HID devices
     */
    class RAZIX_API RZInput
    {
    public:
        // TODO: Use a better API convention for selecting input API implementation
        // TODO: ReWrite this using a simple static function design pattern rather than using Impls, this is cumbersome to extend
        /* Selects the GLFW Input class for polling and reporting input events */
        static void SelectGLFWInputManager();

        // Keyboard
        inline static bool IsKeyPressed(Razix::KeyCode::Key keycode) { return s_Instance->IsKeyPressedImpl(int(keycode)); }
        inline static bool IsKeyReleased(Razix::KeyCode::Key keycode) { return s_Instance->IsKeyReleasedImpl(int(keycode)); }
        inline static bool IsKeyHeld(Razix::KeyCode::Key keycode) { return s_Instance->IsIsKeyHeldImpl(int(keycode)); }

        // Mouse
        inline static bool                IsMouseButtonPressed(Razix::KeyCode::MouseKey button) { return s_Instance->IsMouseButtonPressedImpl(int(button)); }
        inline static bool                IsMouseButtonReleased(Razix::KeyCode::MouseKey button) { return s_Instance->IsMouseButtonReleasedImpl(int(button)); }
        inline static bool                IsMouseButtonHeld(Razix::KeyCode::MouseKey button) { return s_Instance->IsMouseButtonHeldImpl(int(button)); }
        inline static std::pair<f32, f32> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
        inline static f32                 GetMouseX() { return s_Instance->GetMouseXImpl(); }
        inline static f32                 GetMouseY() { return s_Instance->GetMouseYImpl(); }

        // This is written with the PS5 DualSense controller in mind
        // TODO: Support all DualSense buttons
        inline static bool IsGamepadConnected() { return s_Instance->IsGamepadConnectedImpl(); }

        inline static f32 GetJoyLeftStickHorizontal() { return s_Instance->GetJoyLeftStickHorizontalImpl(); }
        inline static f32 GetJoyLeftStickVertical() { return s_Instance->GetJoyLeftStickVerticalImpl(); }
        inline static f32 GetJoyRightStickHorizontal() { return s_Instance->GetJoyRightStickHorizontalImpl(); }
        inline static f32 GetJoyRightStickVertical() { return s_Instance->GetJoyRightStickVerticalImpl(); }
        inline static f32 GetJoyDPadHorizontal() { return s_Instance->GetJoyDPadHorizontalImpl(); }
        inline static f32 GetJoyDPadVertical() { return s_Instance->GetJoyDPadVerticalImpl(); }

        inline static bool IsCrossPressed() { return s_Instance->IsCrossPressedImpl(); }
        inline static bool IsCirclePressed() { return s_Instance->IsCirclePressedImpl(); }
        inline static bool IsTrianglePressed() { return s_Instance->IsTrianglePressedImpl(); }
        inline static bool IsSquarePressed() { return s_Instance->IsSquarePressedImpl(); }

    protected:
        virtual bool                IsKeyPressedImpl(int keycode)         = 0;
        virtual bool                IsKeyReleasedImpl(int keycode)        = 0;
        virtual bool                IsIsKeyHeldImpl(int keycode)          = 0;
        virtual bool                IsMouseButtonPressedImpl(int button)  = 0;
        virtual bool                IsMouseButtonReleasedImpl(int button) = 0;
        virtual bool                IsMouseButtonHeldImpl(int button)     = 0;
        virtual std::pair<f32, f32> GetMousePositionImpl()                = 0;
        virtual f32                 GetMouseXImpl()                       = 0;
        virtual f32                 GetMouseYImpl()                       = 0;

        virtual bool IsGamepadConnectedImpl()         = 0;
        virtual f32  GetJoyLeftStickHorizontalImpl()  = 0;
        virtual f32  GetJoyLeftStickVerticalImpl()    = 0;
        virtual f32  GetJoyRightStickHorizontalImpl() = 0;
        virtual f32  GetJoyRightStickVerticalImpl()   = 0;
        virtual f32  GetJoyDPadHorizontalImpl()       = 0;
        virtual f32  GetJoyDPadVerticalImpl()         = 0;
        virtual bool IsCrossPressedImpl()             = 0;
        virtual bool IsCirclePressedImpl()            = 0;
        virtual bool IsTrianglePressedImpl()          = 0;
        virtual bool IsSquarePressedImpl()            = 0;

    protected:
        static RZInput* s_Instance;
    };
}    // namespace Razix
