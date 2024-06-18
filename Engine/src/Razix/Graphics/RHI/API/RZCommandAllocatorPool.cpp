// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZCommandAllocatorPool.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

namespace Razix {
    namespace Graphics {

        void RZCommandAllocatorPool::Create(void* where, PoolType type)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::VULKAN: new (where) VKCommandPool(); break;
                case Razix::Graphics::RenderAPI::D3D12: new (where) DX12CommandAllocatorPool(); break;
                default: break;
            }
        }

    }    // namespace Graphics
}    // namespace Razix