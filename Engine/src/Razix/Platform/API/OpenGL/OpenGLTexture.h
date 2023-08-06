#pragma once

#include "Razix/Graphics/RHI/API/RZTexture.h"

#ifdef RAZIX_RENDER_API_OPENGL

namespace Razix {
    namespace Graphics {

        class OpenGLTexture2D : public RZTexture2D
        {
        public:
            OpenGLTexture2D(const RZTextureDesc& desc);
            OpenGLTexture2D(const std::string& filePath, const RZTextureDesc& desc);

            void Bind(u32 slot) override;
            void Unbind(u32 slot) override;

            void Release(bool deleteImage) override {}

            void* GetAPIHandlePtr() const override { return (void*) (sz) m_Handle; }
            void  SetData(const void* pixels) override;

        private:
            u32 m_Handle; /* Handle to the OpenGL Texture */

        private:
            u32 load(void* data);
        };
    }    // namespace Graphics
}    // namespace Razix
#endif