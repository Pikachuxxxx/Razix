#pragma once

namespace Razix {
    namespace Gfx {

        // TODO: Delete this! unwanted stuff

        struct RZSamplerDesc;

        struct RZFrameGraphSampler
        {
            typedef RZSamplerDesc Desc;

            void create(const Desc& desc, u32 id, const void* transientAllocator);
            void destroy(u32 id, const void* transientAllocator);

            void preRead(const Desc& desc, uint32_t flags);
            void preWrite(const Desc& desc, uint32_t flags);

            static std::string toString(const Desc& desc);

            Gfx::RZSamplerHandle getHandle() const { return m_SamplerHandle; }

            // public for initializer list support
            Gfx::RZSamplerHandle m_SamplerHandle;
        };
    }    // namespace Gfx
}    // namespace Razix