// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZInput.h"

#include "Razix/Core/App/RZApplication.h"

// Platform-specific includes
#if defined(RAZIX_USE_GLFW_WINDOWS) || defined(RAZIX_PLATFORM_MACOS) || defined(RAZIX_PLATFORM_UNIX)
    #include <GLFW/glfw3.h>
#elif defined(RAZIX_PLATFORM_WINDOWS)
    // Windows native input implementation
    #include <Windows.h>
#endif

namespace Razix {

#if defined(RAZIX_USE_GLFW_WINDOWS) || defined(RAZIX_PLATFORM_MACOS) || defined(RAZIX_PLATFORM_UNIX)
    // GLFW implementation for cross-platform input support

    static GLFWwindow* GetActiveWindow()
    {
        return static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
    }

    static GLFWgamepadstate GetGamepadState()
    {
        GLFWgamepadstate state = {};
        if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
            glfwGetGamepadState(GLFW_JOYSTICK_1, &state);
        }
        return state;
    }

    // Keyboard implementation
    bool RZInput::IsKeyPressed(Razix::KeyCode::Key keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        auto window   = GetActiveWindow();
        int  keyState = glfwGetKey(window, int(keycode));
        return keyState == GLFW_PRESS;
    }

    bool RZInput::IsKeyReleased(Razix::KeyCode::Key keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        auto window   = GetActiveWindow();
        int  keyState = glfwGetKey(window, int(keycode));
        return keyState == GLFW_RELEASE;
    }

    bool RZInput::IsKeyHeld(Razix::KeyCode::Key keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        auto window   = GetActiveWindow();
        int  keyState = glfwGetKey(window, int(keycode));
        return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
    }

    // Mouse implementation
    bool RZInput::IsMouseButtonPressed(Razix::KeyCode::MouseKey button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        bool       isPressed = false;
        static int oldState  = GLFW_RELEASE;
        auto       window    = GetActiveWindow();
        int        newState  = glfwGetMouseButton(window, int(button));
        if (newState == GLFW_PRESS && oldState == GLFW_RELEASE)
            isPressed = true;
        oldState = newState;
        return isPressed;
    }

    bool RZInput::IsMouseButtonReleased(Razix::KeyCode::MouseKey button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        bool       isPressed = false;
        static int oldState  = GLFW_RELEASE;
        auto       window    = GetActiveWindow();
        int        newState  = glfwGetMouseButton(window, int(button));
        if (newState == GLFW_RELEASE && oldState == GLFW_PRESS)
            isPressed = true;
        oldState = newState;
        return isPressed;
    }

    bool RZInput::IsMouseButtonHeld(Razix::KeyCode::MouseKey button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        auto window      = GetActiveWindow();
        int  buttonState = glfwGetMouseButton(window, int(button));
        return buttonState == GLFW_PRESS;
    }

    std::pair<f32, f32> RZInput::GetMousePosition()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        auto window = GetActiveWindow();
        d32  xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return {(f32) xpos, (f32) ypos};
    }

    f32 RZInput::GetMouseX()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        auto [x, y] = GetMousePosition();
        return x;
    }

    f32 RZInput::GetMouseY()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        auto [x, y] = GetMousePosition();
        return y;
    }

    // Gamepad implementation
    bool RZInput::IsGamepadConnected()
    {
        return glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1);
    }

    f32 RZInput::GetJoyLeftStickHorizontal()
    {
        return GetGamepadState().axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    }

    f32 RZInput::GetJoyLeftStickVertical()
    {
        return GetGamepadState().axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
    }

    f32 RZInput::GetJoyRightStickHorizontal()
    {
        return GetGamepadState().axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
    }

    f32 RZInput::GetJoyRightStickVertical()
    {
        return GetGamepadState().axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
    }

    f32 RZInput::GetJoyDPadHorizontal()
    {
        const auto state = GetGamepadState();
        int        left  = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS ? -1 : 0;
        int        right = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS ? 1 : 0;
        return static_cast<f32>(left + right);
    }

    f32 RZInput::GetJoyDPadVertical()
    {
        const auto state = GetGamepadState();
        int        up    = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS ? -1 : 0;
        int        down  = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS ? 1 : 0;
        return static_cast<f32>(up + down);
    }

    bool RZInput::IsCrossPressed()
    {
        return GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS;
    }

    bool RZInput::IsCirclePressed()
    {
        return GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS;
    }

    bool RZInput::IsTrianglePressed()
    {
        return GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS;
    }

    bool RZInput::IsSquarePressed()
    {
        return GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS;
    }

#elif defined(RAZIX_PLATFORM_WINDOWS)
    // Windows-specific implementation (placeholder for future Windows native input)
    // For now, we'll use stub implementations

    bool RZInput::IsKeyPressed(Razix::KeyCode::Key keycode)
    {
        // TODO: Implement Windows-specific keyboard input
        return false;
    }

    bool RZInput::IsKeyReleased(Razix::KeyCode::Key keycode)
    {
        // TODO: Implement Windows-specific keyboard input
        return false;
    }

    bool RZInput::IsKeyHeld(Razix::KeyCode::Key keycode)
    {
        // TODO: Implement Windows-specific keyboard input
        return false;
    }

    bool RZInput::IsMouseButtonPressed(Razix::KeyCode::MouseKey button)
    {
        // TODO: Implement Windows-specific mouse input
        return false;
    }

    bool RZInput::IsMouseButtonReleased(Razix::KeyCode::MouseKey button)
    {
        // TODO: Implement Windows-specific mouse input
        return false;
    }

    bool RZInput::IsMouseButtonHeld(Razix::KeyCode::MouseKey button)
    {
        // TODO: Implement Windows-specific mouse input
        return false;
    }

    std::pair<f32, f32> RZInput::GetMousePosition()
    {
        // TODO: Implement Windows-specific mouse position
        return {0.0f, 0.0f};
    }

    f32 RZInput::GetMouseX()
    {
        auto [x, y] = GetMousePosition();
        return x;
    }

    f32 RZInput::GetMouseY()
    {
        auto [x, y] = GetMousePosition();
        return y;
    }

    bool RZInput::IsGamepadConnected()
    {
        // TODO: Implement Windows-specific gamepad detection
        return false;
    }

    f32 RZInput::GetJoyLeftStickHorizontal()
    {
        // TODO: Implement Windows-specific gamepad input
        return 0.0f;
    }

    f32 RZInput::GetJoyLeftStickVertical()
    {
        // TODO: Implement Windows-specific gamepad input
        return 0.0f;
    }

    f32 RZInput::GetJoyRightStickHorizontal()
    {
        // TODO: Implement Windows-specific gamepad input
        return 0.0f;
    }

    f32 RZInput::GetJoyRightStickVertical()
    {
        // TODO: Implement Windows-specific gamepad input
        return 0.0f;
    }

    f32 RZInput::GetJoyDPadHorizontal()
    {
        // TODO: Implement Windows-specific gamepad input
        return 0.0f;
    }

    f32 RZInput::GetJoyDPadVertical()
    {
        // TODO: Implement Windows-specific gamepad input
        return 0.0f;
    }

    bool RZInput::IsCrossPressed()
    {
        // TODO: Implement Windows-specific gamepad input
        return false;
    }

    bool RZInput::IsCirclePressed()
    {
        // TODO: Implement Windows-specific gamepad input
        return false;
    }

    bool RZInput::IsTrianglePressed()
    {
        // TODO: Implement Windows-specific gamepad input
        return false;
    }

    bool RZInput::IsSquarePressed()
    {
        // TODO: Implement Windows-specific gamepad input
        return false;
    }

#else
    #error "Unsupported platform for input system"
#endif

}    // namespace Razix
