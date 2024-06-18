// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZCommandAllocatorPool.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKCommandPool.h"
    #include "Razix/Platform/API/Vulkan/VKDevice.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include "Razix/Platform/API/DirectX12/DX12CommandAllocatorPool.h"
#endif

#include "Razix/Graphics/Resources/IRZResource.h"

namespace Razix {
    namespace Graphics {

        u32 RZCommandAllocatorPool::GetInstanceSize()
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::VULKAN: return sizeof(VKCommandPool); break;
                case Razix::Graphics::RenderAPI::D3D12: return sizeof(DX12CommandAllocatorPool); break;
                default: return sizeof(RZCommandAllocatorPool); break;
            }
        }

        Razix::Graphics::RZDrawCommandBufferHandle RZCommandAllocatorPool::allocateCommandBuffer()
        {
            // Create a command buffers and push it to the ring buffer
            auto& commandPoolHandle = getHandle();
            RAZIX_CORE_ASSERT(commandPoolHandle.isValid(), "Invalid command pool handle!.");
            return RZResourceManager::Get().createDrawCommandBuffer(commandPoolHandle);
        }

        void RZCommandAllocatorPool::Create(void* where, PoolType type)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::VULKAN:
                    if (type == PoolType::kGraphics) {
                        new (where) VKCommandPool(VKDevice::Get().getPhysicalDevice()->getGraphicsQueueFamilyIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
                        return;
                    }
                    break;
                case Razix::Graphics::RenderAPI::D3D12:
                    if (type == PoolType::kGraphics) {
                        new (where) DX12CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE_DIRECT);
                        return;
                    }
                    break;
                default: break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix