#pragma once

#include "Razix/Graphics/RHI/API/RZFramebuffer.h"

namespace Razix {
    namespace Graphics {

        class OpenGLFrameBuffer : public RZFramebuffer
        {
        public:
            OpenGLFrameBuffer(const FramebufferInfo& frameBufInfo);
            ~OpenGLFrameBuffer();
                
            void Bind() override;
            void Unbind() override;
            void Destroy() override;

        private:
            uint32_t m_FBO;
            uint32_t m_RBO;
            bool     isScreen = true;
        };
    }    // namespace Graphics
}    // namespace Razix
