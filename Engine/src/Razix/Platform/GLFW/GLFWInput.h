#pragma once

#include "Razix/Core/OS/RZInput.h"

namespace Razix {
    /**
     * GLFW-based input implementation for cross-platform support
     * Provides input handling for Windows, macOS, Linux using GLFW
     */
    class RAZIX_API GLFWInput : public RZInput
    {
    public:
        GLFWInput() = default;
        virtual ~GLFWInput() = default;

    protected:
        // Keyboard implementation
        bool IsKeyPressedImpl(int keycode) override;
        bool IsKeyReleasedImpl(int keycode) override;
        bool IsIsKeyHeldImpl(int keycode) override;

        // Mouse implementation
        bool IsMouseButtonPressedImpl(int button) override;
        bool IsMouseButtonReleasedImpl(int button) override;
        bool IsMouseButtonHeldImpl(int button) override;
        std::pair<f32, f32> GetMousePositionImpl() override;
        f32 GetMouseXImpl() override;
        f32 GetMouseYImpl() override;

        // Gamepad implementation
        bool IsGamepadConnectedImpl() override;
        f32  GetJoyLeftStickHorizontalImpl() override;
        f32  GetJoyLeftStickVerticalImpl() override;
        f32  GetJoyRightStickHorizontalImpl() override;
        f32  GetJoyRightStickVerticalImpl() override;
        f32  GetJoyDPadHorizontalImpl() override;
        f32  GetJoyDPadVerticalImpl() override;
        bool IsCrossPressedImpl() override;
        bool IsCirclePressedImpl() override;
        bool IsTrianglePressedImpl() override;
        bool IsSquarePressedImpl() override;

    private:
        // Helper methods for GLFW-specific functionality
        static GLFWwindow* GetActiveWindow();
        static GLFWgamepadstate GetGamepadState();
    };

}    // namespace Razix
