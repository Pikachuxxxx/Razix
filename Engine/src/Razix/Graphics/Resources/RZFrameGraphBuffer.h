#pragma once

#include "Razix/Graphics/RHI/API/RZAPIDesc.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            class RZFrameGraphBuffer
            {
            public:
                typedef RZBufferDesc Desc;

                void create(const Desc& desc, void* transientAllocator);
                void destroy(const Desc& desc, void* transientAllocator);

                static std::string toString(const Desc& desc);

                void preRead(const Desc& desc, uint32_t flags);
                void preWrite(const Desc& desc, uint32_t flags);

                Graphics::RZUniformBufferHandle getHandle() { return m_BufferHandle; }

            public:
                Graphics::RZUniformBufferHandle m_BufferHandle;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix