#include "rzxpch.h"
#include "GLFWInput.h"

#include "Razix/Core/RZApplication.h"
#include <GLFW/glfw3.h>

#ifdef RAZIX_RENDER_API_OPENGL
#include "Razix/Platform/GLFW/GLFWInput.h"
#endif // RAZIX_RENDER_API_OPENGL

namespace Razix
{
    // Temporarily creating Input directly from GLFW
	Razix::RZInput* Razix::RZInput::sInstance = nullptr;

    bool GLFWInput::IsKeyPressedImpl(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        int keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_PRESS;
    }

    bool GLFWInput::IsKeyReleasedImpl(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        int keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_RELEASE;
    }

    bool GLFWInput::IsIsKeyHeldImpl(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        int keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
    }

    bool GLFWInput::IsMouseButtonPressedImpl(int button)
    {
        bool isPressed = false;
        static int oldState = GLFW_RELEASE;
        auto window = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        int newState  = glfwGetMouseButton(window, button);
        if (newState == GLFW_PRESS && oldState == GLFW_RELEASE)
            isPressed = true;
        oldState = newState;
        return isPressed;
    }

    bool GLFWInput::IsMouseButtonReleasedImpl(int button)
    {
        bool isPressed = false;
        static int oldState = GLFW_RELEASE;
        auto window = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        int newState = glfwGetMouseButton(window, button);
        if (newState == GLFW_RELEASE && oldState == GLFW_PRESS)
            isPressed = true;
        oldState = newState;
        return isPressed;
    }

    bool GLFWInput::IsMouseButtonHeldImpl(int button)
    {
        auto window = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        int buttonState = glfwGetMouseButton(window, button);
        return buttonState == GLFW_PRESS;
    }

    std::pair<float, float> GLFWInput::GetMousePositionImpl()
    {
        auto window = static_cast<GLFWwindow*>(RZApplication::Get().getWindow()->GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return { (float)xpos, (float)ypos };
    }

    float GLFWInput::GetMouseXImpl()
    {
        auto [x, y] = GetMousePositionImpl();
        return x;
    }

    float GLFWInput::GetMouseYImpl()
    {
        auto [x, y] = GetMousePositionImpl();
        return y;
    }

}