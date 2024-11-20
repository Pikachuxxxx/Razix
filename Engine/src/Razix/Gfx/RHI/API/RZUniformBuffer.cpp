// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZUniformBuffer.h"

#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"

#include "Razix/Platform/API/Vulkan/VKUniformBuffer.h"

namespace Razix {
    namespace Gfx {

        u32 RZUniformBuffer::GetInstanceSize()
        {
            switch (Gfx::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Gfx::RenderAPI::VULKAN: return sizeof(VKUniformBuffer); break;
                //case Razix::Gfx::RenderAPI::D3D12: return sizeof(DX12ConstantBuffer); break;
                default: return sizeof(RZCommandPool); break;
            }
        }

        void RZUniformBuffer::Create(void* where, const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Gfx::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Gfx::RenderAPI::VULKAN: new (where) VKUniformBuffer(desc RZ_DEBUG_E_ARG_NAME); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Gfx::RenderAPI::D3D12:
#endif
                default: break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix