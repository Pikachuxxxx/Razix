// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphSampler.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

namespace Razix {
    namespace Gfx {
        namespace FrameGraph {

            void RZFrameGraphSampler::create(const Desc& desc, void* transientAllocator)
            {
                if (!m_SamplerHandle.isValid())
                    m_SamplerHandle = RZResourceManager::Get().createSampler(desc);
            }

            void RZFrameGraphSampler::destroy(const Desc& desc, void* transientAllocator)
            {
                RZResourceManager::Get().destroySampler(m_SamplerHandle);
            }

            std::string RZFrameGraphSampler::toString(const Desc& desc)
            {
                return desc.name;
            }

            void RZFrameGraphSampler::preRead(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            }

            void RZFrameGraphSampler::preWrite(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            }
        }    // namespace FrameGraph
    }        // namespace Gfx
}    // namespace Razix
