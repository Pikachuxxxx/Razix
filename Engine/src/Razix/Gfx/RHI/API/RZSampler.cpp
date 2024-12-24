// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSampler.h"

#include "Razix/Gfx/Resources/IRZResource.h"

#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKSampler.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include "Razix/Platform/API/DirectX12/DX12Sampler.h"
#endif

namespace Razix {
    namespace Gfx {

        GET_INSTANCE_SIZE_IMPL(Sampler)

        void RZSampler::Create(void* where, const RZSamplerDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Gfx::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Gfx::RenderAPI::VULKAN: new (where) VKSampler(desc RZ_DEBUG_E_ARG_NAME); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Gfx::RenderAPI::D3D12: new (where) DX12Sampler(desc RZ_DEBUG_E_ARG_NAME); break;
#endif
                default: break;
            }
        }

        void RZSampler::CreateSamplerPresets()
        {
            for (u32 i = 0; i < u32(SamplerPresets::COUNT); i++)
                g_SamplerPresets[i] = RZResourceManager::Get().createSampler(g_SamplerCreateDescPresets[i]);
        }

        void RZSampler::DestroySamplerPresets()
        {
            for (u32 i = 0; i < u32(SamplerPresets::COUNT); i++)
                RZResourceManager::Get().destroySampler(g_SamplerPresets[i]);
        }

    }    // namespace Gfx
}    // namespace Razix