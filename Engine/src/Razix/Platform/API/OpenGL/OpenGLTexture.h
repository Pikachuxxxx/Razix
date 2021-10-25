#pragma once

#include "Razix/Graphics/API/Texture.h"

#ifdef RAZIX_RENDER_API_OPENGL

namespace Razix {
    namespace Graphics {

        class OpenGLTexture : public Texture
        {
        public:
            OpenGLTexture();

        protected:
            void Bind() override;


            void Unbind() override;


            void* GetHandle() const override;

        };

    }
}
#endif