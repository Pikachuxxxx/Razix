#pragma once

#include "Razix/Graphics/RHI/API/RZTexture.h"

#ifdef RAZIX_RENDER_API_OPENGL

namespace Razix {
    namespace Graphics {

        class OpenGLTexture : public RZTexture
        {
        public:
            OpenGLTexture(const RZTextureDesc& desc);
            OpenGLTexture(const RZTextureDesc& desc, const std::string& filePath);

            void Bind(u32 slot) override;
            void Unbind(u32 slot) override;

            void DestroyResource() override {}

            void* GetAPIHandlePtr() const override { return (void*) (sz) m_Handle; }
            void  SetData(const void* pixels) override;

            int32_t ReadPixels(u32 x, u32 y) override;

        private:
            u32 m_Handle; /* Handle to the OpenGL Texture */

        private:
            u32 load(void* data);
        };
    }    // namespace Graphics
}    // namespace Razix
#endif