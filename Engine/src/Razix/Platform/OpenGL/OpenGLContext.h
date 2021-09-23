#pragma once

#include "Razix/Graphics/API/GraphicsContext.h"

struct GLFWwindow;

namespace Razix
{

    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(GLFWwindow* windowHandle);

        virtual void Init() override;
        virtual void SwapBuffers() override;
    private:
        GLFWwindow* m_WindowHandle;
    };

}

