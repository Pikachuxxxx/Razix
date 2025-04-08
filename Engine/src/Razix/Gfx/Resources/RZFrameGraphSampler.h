#pragma once

namespace Razix {
    namespace Gfx {

        struct RZSamplerDesc;

        namespace FrameGraph {

            struct RZFrameGraphSampler
            {
                typedef RZSamplerDesc Desc;

                void create(const Desc& desc, void* transientAllocator);
                void destroy(const Desc& desc, void* transientAllocator);

                void preRead(const Desc& desc, uint32_t flags);
                void preWrite(const Desc& desc, uint32_t flags);

                static std::string toString(const Desc& desc);

                Gfx::RZSamplerHandle getHandle() const { return m_SamplerHandle; }

                // public for initializer list support
                Gfx::RZSamplerHandle m_SamplerHandle;
            };
        }    // namespace FrameGraph
    }        // namespace Gfx
}    // namespace Razix