// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZDrawCommandBuffer.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLCommandBuffer.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKDrawCommandBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKUtilities.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include "Razix/Platform/API/DirectX12/DX12Utilities.h"
    #include "Razix/Platform/API/DirectX12/DX12DrawCommandBuffer.h"
#endif

namespace Razix {
    namespace Graphics {

        GET_INSTANCE_SIZE_IMPL(DrawCommandBuffer)

        RZDrawCommandBufferHandle RZDrawCommandBuffer::BeginSingleTimeCommandBuffer()
        {
            RZHandle<RZDrawCommandBuffer> handle;
            void*                         where = RZResourceManager::Get().getPool<RZDrawCommandBuffer>().obtain(handle);

#ifdef RAZIX_RENDER_API_VULKAN
            if (Graphics::RZGraphicsContext::GetRenderAPI() == RenderAPI::VULKAN) {
                auto                 vkCmdBuffer = VKUtilities::BeginSingleTimeCommandBuffer();
                VKDrawCommandBuffer* cmdBuffer   = new (where) VKDrawCommandBuffer(vkCmdBuffer);
            }
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
            if (Graphics::RZGraphicsContext::GetRenderAPI() == RenderAPI::D3D12) {
                auto                   d3d12CmdBuffer = DX12Utilities::BeginSingleTimeCommandBuffer();
                DX12DrawCommandBuffer* cmdBuffer      = new (where) DX12DrawCommandBuffer(d3d12CmdBuffer);
            }
#endif
            IRZResource<RZDrawCommandBuffer>* resource = (IRZResource<RZDrawCommandBuffer>*) where;
            resource->setName("Single Time Command Buffer");
            resource->setHandle(handle);
            return handle;
        }

        void RZDrawCommandBuffer::EndSingleTimeCommandBuffer(RZDrawCommandBufferHandle cmdBuffer)
        {
            auto cmdBufferResource = RZResourceManager::Get().getDrawCommandBufferResource(cmdBuffer);
#ifdef RAZIX_RENDER_API_VULKAN
            if (Graphics::RZGraphicsContext::GetRenderAPI() == RenderAPI::VULKAN)
                return VKUtilities::EndSingleTimeCommandBuffer(static_cast<VKDrawCommandBuffer*>(cmdBufferResource)->getBuffer());
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
            if (Graphics::RZGraphicsContext::GetRenderAPI() == RenderAPI::D3D12)
                return DX12Utilities::EndSingleTimeCommandBuffer(static_cast<DX12DrawCommandBuffer*>(cmdBufferResource)->getD3DCommandList());
#endif
        }

        void RZDrawCommandBuffer::Create(void* where, RZCommandPoolHandle pool)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the command pool API handle
            auto poolApiHandle = RZResourceManager::Get().getCommandPoolResource(pool)->getAPIHandle();

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Graphics::RenderAPI::OPENGL: new (where) OpenGLCommandBuffer(); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Graphics::RenderAPI::VULKAN: new (where) VKDrawCommandBuffer(*(VkCommandPool*) (poolApiHandle)); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Graphics::RenderAPI::D3D12: new (where) DX12DrawCommandBuffer((ID3D12CommandAllocator*) poolApiHandle); break;
#endif
                default: break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix