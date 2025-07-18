// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphSampler.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

namespace Razix {
    namespace Gfx {
        void RZFrameGraphSampler::create(const Desc& desc, u32 id, const void* transientAllocator)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            //if (transientAllocator)
            //    m_SamplerHandle = TRANSIENT_ALLOCATOR_CAST(transientAllocator)->acquireTransientSampler(desc, id);
            //else {
            //    // If no transient allocator is provided, we create a imported persistent resource only ONCE!
            //    if (!m_SamplerHandle.isValid())
            //        m_SamplerHandle = RZResourceManager::Get().createSampler(desc);
            //}
        }

        void RZFrameGraphSampler::destroy(u32 id, const void* transientAllocator)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            //if (transientAllocator)
            //    TRANSIENT_ALLOCATOR_CAST(transientAllocator)->releaseTransientSampler(m_SamplerHandle, id);
            //else {
            //    // If no transient allocator is provided, we create a imported persistent resource only ONCE!
            //    if (m_SamplerHandle.isValid())
            //        RZResourceManager::Get().destroySampler(m_SamplerHandle);
            //}
        }

        void RZFrameGraphSampler::preRead(const Desc& desc, uint32_t flags)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void RZFrameGraphSampler::preWrite(const Desc& desc, uint32_t flags)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        std::string RZFrameGraphSampler::toString(const Desc& desc)
        {
            //return desc.name;
            return "";
        }
    }    // namespace Gfx
}    // namespace Razix
