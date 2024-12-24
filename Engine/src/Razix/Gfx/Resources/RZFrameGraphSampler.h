#pragma once

#include "Razix/Gfx/RHI/API/RZAPIDesc.h"

namespace Razix {
    namespace Gfx {

        struct RZSamplerDesc;

        namespace FrameGraph {

            class RZFrameGraphSampler
            {
            public:
                typedef RZSamplerDesc Desc;

                void create(const Desc& desc, void* transientAllocator);
                void destroy(const Desc& desc, void* transientAllocator);

                static std::string toString(const Desc& desc);

                void preRead(const Desc& desc, uint32_t flags);
                void preWrite(const Desc& desc, uint32_t flags);

                Gfx::RZSamplerHandle getHandle() const { return m_SamplerHandle; }

            public:
                Gfx::RZSamplerHandle m_SamplerHandle;
            };
        }    // namespace FrameGraph
    }        // namespace Gfx
}    // namespace Razix