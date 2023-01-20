#pragma once

#include "Razix/Graphics/RHI/API/RZTexture.h"

#ifdef RAZIX_RENDER_API_OPENGL

namespace Razix {
    namespace Graphics {

        class OpenGLTexture2D : public RZTexture2D
        {
        public:
            OpenGLTexture2D(const std::string& name, uint32_t width, uint32_t height, void* data, Format format, Wrapping wrapMode, Filtering filterMode);
            OpenGLTexture2D(const std::string& filePath, const std::string& name, Wrapping wrapMode, Filtering filterMode);

            void Bind(uint32_t slot) override;
            void Unbind(uint32_t slot) override;

            void Release(bool deleteImage) override {}

            void* GetHandle() const override { return (void*) (size_t) m_Handle; }
            void  SetData(const void* pixels) override;

        private:
            uint32_t m_Handle; /* Handle to the OpenGL Texture */

        private:
            uint32_t load(void* data);
        };
    }    // namespace Graphics
}    // namespace Razix
#endif