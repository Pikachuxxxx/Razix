#include "rzxpch.h"
#include "GLFWInput.h"

#include "Razix/Core/Application.h"
#include <GLFW/glfw3.h>

#ifdef RAZIX_RENDER_API_OPENGL
#include "Razix/Platform/OpenGL/GLFWInput.h"
#endif // RAZIX_RENDER_API_OPENGL

namespace Razix
{
    // Temporarily creating Input directly from GLFW
	Razix::Input* Razix::Input::sInstance = new GLFWInput();

    bool GLFWInput::IsKeyPressedImpl(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
        int keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_PRESS;
    }

    bool GLFWInput::IsKeyReleasedImpl(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
        int keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_RELEASE;
    }

    bool GLFWInput::IsIsKeyHeldImpl(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
        int keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
    }

    bool GLFWInput::IsMouseButtonPressedImpl(int button)
    {
        auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
        int buttonState = glfwGetMouseButton(window, button);
        return buttonState == GLFW_PRESS;
    }

    std::pair<float, float> GLFWInput::GetMousePositionImpl()
    {
        auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
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