#pragma once

#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            enum class BufferType
            {
                Buffer_Uniform,
                Buffer_Storage
            };

            class RZFrameGraphBuffer
            {
            public:
                struct Desc
                {
                    std::string name;
                    u32         size;
                };

                void create(const Desc& desc, void* transientAllocator);
                void destroy(const Desc& desc, void* transientAllocator);

                static std::string toString(const Desc& desc);

                void preRead(const Desc& desc, uint32_t flags);
                void preWrite(const Desc& desc, uint32_t flags);

                Graphics::RZUniformBuffer* getHandle() { return m_Buffer; }

            public:
                Graphics::RZUniformBuffer* m_Buffer;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix