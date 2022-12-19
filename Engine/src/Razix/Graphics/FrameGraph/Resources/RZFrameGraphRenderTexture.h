#pragma once

#include "Razix/Graphics/API/RZTexture.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            class RZFrameGraphRenderTexture
            {
            public:
                struct Desc
                {
                    // TODO: Name only exists in debug mode or use the name while creating the resource
                    std::string                 name;
                    glm::vec2                   extent;
                    Graphics::RZTexture::Format format;
                };

                void create(const Desc& desc, void* allocator);
                void destroy(const Desc& desc, void* allocator);

                static std::string toString(const Desc& desc);

            private:
                Graphics::RZRenderTexture* m_RenderTarget;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix