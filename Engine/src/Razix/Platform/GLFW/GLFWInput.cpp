// clang-format off
#include "rzxpch.h"
// clang-format on
#include "GLFWInput.h"

#include "Razix/Core/App/RZApplication.h"
#include <GLFW/glfw3.h>

namespace Razix {

    // Helper methods for GLFW functionality
    GLFWwindow* GLFWInput::GetActiveWindow()
    {
        return static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
    }

    GLFWgamepadstate GLFWInput::GetGamepadState()
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
        auto window   = GLFWInput::GetActiveWindow();
        int  keyState = glfwGetKey(window, int(keycode));
        return keyState == GLFW_PRESS;
    }

    bool RZInput::IsKeyReleased(Razix::KeyCode::Key keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        auto window   = GLFWInput::GetActiveWindow();
        int  keyState = glfwGetKey(window, int(keycode));
        return keyState == GLFW_RELEASE;
    }

    bool RZInput::IsKeyHeld(Razix::KeyCode::Key keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        auto window   = GLFWInput::GetActiveWindow();
        int  keyState = glfwGetKey(window, int(keycode));
        return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
    }

    // Mouse implementation
    bool RZInput::IsMouseButtonPressed(Razix::KeyCode::MouseKey button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        bool       isPressed = false;
        static int oldState  = GLFW_RELEASE;
        auto       window    = GLFWInput::GetActiveWindow();
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
        auto       window    = GLFWInput::GetActiveWindow();
        int        newState  = glfwGetMouseButton(window, int(button));
        if (newState == GLFW_RELEASE && oldState == GLFW_PRESS)
            isPressed = true;
        oldState = newState;
        return isPressed;
    }

    bool RZInput::IsMouseButtonHeld(Razix::KeyCode::MouseKey button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        auto window      = GLFWInput::GetActiveWindow();
        int  buttonState = glfwGetMouseButton(window, int(button));
        return buttonState == GLFW_PRESS;
    }

    std::pair<f32, f32> RZInput::GetMousePosition()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        auto window = GLFWInput::GetActiveWindow();
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
        return GLFWInput::GetGamepadState().axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    }

    f32 RZInput::GetJoyLeftStickVertical()
    {
        return GLFWInput::GetGamepadState().axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
    }

    f32 RZInput::GetJoyRightStickHorizontal()
    {
        return GLFWInput::GetGamepadState().axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
    }

    f32 RZInput::GetJoyRightStickVertical()
    {
        return GLFWInput::GetGamepadState().axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
    }

    f32 RZInput::GetJoyDPadHorizontal()
    {
        const auto state = GLFWInput::GetGamepadState();
        int        left  = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS ? -1 : 0;
        int        right = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS ? 1 : 0;
        return static_cast<f32>(left + right);
    }

    f32 RZInput::GetJoyDPadVertical()
    {
        const auto state = GLFWInput::GetGamepadState();
        int        up    = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS ? -1 : 0;
        int        down  = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS ? 1 : 0;
        return static_cast<f32>(up + down);
    }

    bool RZInput::IsCrossPressed()
    {
        return GLFWInput::GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS;
    }

    bool RZInput::IsCirclePressed()
    {
        return GLFWInput::GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS;
    }

    bool RZInput::IsTrianglePressed()
    {
        return GLFWInput::GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS;
    }

    bool RZInput::IsSquarePressed()
    {
        return GLFWInput::GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS;
    }

}    // namespace Razix