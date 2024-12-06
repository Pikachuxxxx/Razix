// clang-format off
#include "rzxpch.h"
// clang-format on
#include "GLFWInput.h"

#include "Razix/Core/RZApplication.h"
#include <GLFW/glfw3.h>

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/GLFW/GLFWInput.h"
#endif    // RAZIX_RENDER_API_OPENGL

namespace Razix {
    // Temporarily creating Input directly from GLFW
    Razix::RZInput* Razix::RZInput::sInstance = nullptr;

    bool GLFWInput::IsKeyPressedImpl(int keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        auto window   = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        int  keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_PRESS;
    }

    bool GLFWInput::IsKeyReleasedImpl(int keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        auto window   = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        int  keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_RELEASE;
    }

    bool GLFWInput::IsIsKeyHeldImpl(int keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        auto window   = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        int  keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
    }

    bool GLFWInput::IsMouseButtonPressedImpl(int button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        bool       isPressed = false;
        static int oldState  = GLFW_RELEASE;
        auto       window    = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
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
        auto       window    = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        int        newState  = glfwGetMouseButton(window, button);
        if (newState == GLFW_RELEASE && oldState == GLFW_PRESS)
            isPressed = true;
        oldState = newState;
        return isPressed;
    }

    bool GLFWInput::IsMouseButtonHeldImpl(int button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        auto window      = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        int  buttonState = glfwGetMouseButton(window, button);
        return buttonState == GLFW_PRESS;
    }

    std::pair<f32, f32> GLFWInput::GetMousePositionImpl()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        auto   window = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        d32 xpos, ypos;
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

}    // namespace Razix