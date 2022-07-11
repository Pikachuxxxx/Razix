#pragma once

#include "Razix/Graphics/API/RZGraphicsContext.h"
#include "Razix/Platform/API/OpenGL/OpenGLSwapchain.h"

struct GLFWwindow;

namespace Razix {
    namespace Graphics {
        class OpenGLContext : public RZGraphicsContext
        {
        public:
            OpenGLContext(GLFWwindow* windowHandle);

            virtual void Init() override;
            virtual void Destroy() override;
            virtual void ClearWithColor(float r, float g, float b) override;

            static OpenGLContext* Get() { return static_cast<OpenGLContext*>(s_Context); }

            inline GLFWwindow* getGLFWWindow() const { return m_Window; }
            inline OpenGLSwapchain* getSwapchain() const { return m_Swapchain; }

        private:
            GLFWwindow* m_Window;
            OpenGLSwapchain* m_Swapchain;
        };
    }    // namespace Graphics

}    // namespace Razix
