#include "rzxpch.h"
#include "WindowsInput.h"

#include "Razix/Application.h"
#include <GLFW/glfw3.h>

namespace Razix
{
    Input* Input::sInstance = new WindowsInput();

    bool WindowsInput::IsKeyPressedImpl(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
        int keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_PRESS;
    }

    bool WindowsInput::IsKeyReleasedImpl(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
        int keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_RELEASE;
    }

    bool WindowsInput::IsIsKeyHeldImpl(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
        int keyState = glfwGetKey(window, keycode);
        return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
    }

    bool WindowsInput::IsMouseButtonPressedImpl(int button)
    {
        auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
        int buttonState = glfwGetMouseButton(window, button);
        return buttonState == GLFW_PRESS;
    }

    std::pair<float, float> WindowsInput::GetMousePositionImpl()
    {
        auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return { (float)xpos, (float)ypos };
    }

    float WindowsInput::GetMouseXImpl()
    {
        auto [x, y] = GetMousePositionImpl();
        return x;
    }

    float WindowsInput::GetMouseYImpl()
    {
        auto [x, y] = GetMousePositionImpl();
        return y;
    }

}