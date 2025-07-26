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
    bool GLFWInput::IsKeyPressedImpl(int keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        auto window   = GetActiveWindow();
        int  keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_PRESS;
    }

    bool GLFWInput::IsKeyReleasedImpl(int keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        auto window   = GetActiveWindow();
        int  keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_RELEASE;
    }

    bool GLFWInput::IsIsKeyHeldImpl(int keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        auto window   = GetActiveWindow();
        int  keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
    }

    // Mouse implementation
    bool GLFWInput::IsMouseButtonPressedImpl(int button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        bool       isPressed = false;
        static int oldState  = GLFW_RELEASE;
        auto       window    = GetActiveWindow();
        int        newState  = glfwGetMouseButton(window, button);
        if (newState == GLFW_PRESS && oldState == GLFW_RELEASE)
            isPressed = true;
        oldState = newState;
        return isPressed;
    }

    bool GLFWInput::IsMouseButtonReleasedImpl(int button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        bool       isPressed = false;
        static int oldState  = GLFW_RELEASE;
        auto       window    = GetActiveWindow();
        int        newState  = glfwGetMouseButton(window, button);
        if (newState == GLFW_RELEASE && oldState == GLFW_PRESS)
            isPressed = true;
        oldState = newState;
        return isPressed;
    }

    bool GLFWInput::IsMouseButtonHeldImpl(int button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        auto window      = GetActiveWindow();
        int  buttonState = glfwGetMouseButton(window, button);
        return buttonState == GLFW_PRESS;
    }

    std::pair<f32, f32> GLFWInput::GetMousePositionImpl()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        auto window = GetActiveWindow();
        d32  xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return {(f32) xpos, (f32) ypos};
    }

    f32 GLFWInput::GetMouseXImpl()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        auto [x, y] = GetMousePositionImpl();
        return x;
    }

    f32 GLFWInput::GetMouseYImpl()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        auto [x, y] = GetMousePositionImpl();
        return y;
    }

    // Gamepad implementation
    bool GLFWInput::IsGamepadConnectedImpl()
    {
        return glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1);
    }

    f32 GLFWInput::GetJoyLeftStickHorizontalImpl()
    {
        return GetGamepadState().axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    }

    f32 GLFWInput::GetJoyLeftStickVerticalImpl()
    {
        return GetGamepadState().axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
    }

    f32 GLFWInput::GetJoyRightStickHorizontalImpl()
    {
        return GetGamepadState().axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
    }

    f32 GLFWInput::GetJoyRightStickVerticalImpl()
    {
        return GetGamepadState().axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
    }

    f32 GLFWInput::GetJoyDPadHorizontalImpl()
    {
        const auto state = GetGamepadState();
        int        left  = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS ? -1 : 0;
        int        right = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS ? 1 : 0;
        return static_cast<f32>(left + right);
    }

    f32 GLFWInput::GetJoyDPadVerticalImpl()
    {
        const auto state = GetGamepadState();
        int        up    = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS ? -1 : 0;
        int        down  = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS ? 1 : 0;
        return static_cast<f32>(up + down);
    }

    bool GLFWInput::IsCrossPressedImpl()
    {
        return GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS;
    }

    bool GLFWInput::IsCirclePressedImpl()
    {
        return GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS;
    }

    bool GLFWInput::IsTrianglePressedImpl()
    {
        return GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS;
    }

    bool GLFWInput::IsSquarePressedImpl()
    {
        return GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS;
    }

}    // namespace Razix
    {
        return GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS;
    }

    bool GLFWInput::IsTrianglePressedImpl()
    {
        return GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS;
    }

    bool GLFWInput::IsSquarePressedImpl()
    {
        return GetGamepadState().buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS;
    }

}    // namespace Razix