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
    #include "Razix/Platform/API/DirectX12/DX12DrawCommandBuffer.h"
#endif

namespace Razix {
    namespace Graphics {

        u32 RZDrawCommandBuffer::GetInstanceSize()
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::VULKAN: return sizeof(RZDrawCommandBuffer); break;
                case Razix::Graphics::RenderAPI::D3D12: return sizeof(RZDrawCommandBuffer); break;
                default: return sizeof(RZCommandAllocatorPool); break;
            }
        }

        RZDrawCommandBuffer* RZDrawCommandBuffer::BeginSingleTimeCommandBuffer()
        {
            auto                 vkCmdBuffer = VKUtilities::BeginSingleTimeCommandBuffer();
            void*                where       = Memory::RZMalloc(sizeof(VKDrawCommandBuffer));
            VKDrawCommandBuffer* cmdBuffer   = new (where) VKDrawCommandBuffer(vkCmdBuffer);
            return (RZDrawCommandBuffer*) cmdBuffer;
        }

        void RZDrawCommandBuffer::EndSingleTimeCommandBuffer(RZDrawCommandBufferHandle cmdBuffer)
        {
            return VKUtilities::EndSingleTimeCommandBuffer(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer());
        }

        void RZDrawCommandBuffer::Create(void* where, RZCommandAllocatorPoolHandle pool)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the command pool API handle
            auto poolApiHandle = RZResourceManager::Get().getCommandAllocator(pool)->getAPIHandle();

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: new (where) OpenGLCommandBuffer(); break;
                case Razix::Graphics::RenderAPI::VULKAN: new (where) VKDrawCommandBuffer(*(VkCommandPool*) (poolApiHandle)); break;
                case Razix::Graphics::RenderAPI::D3D12: new (where) DX12DrawCommandBuffer((ID3D12CommandAllocator*) poolApiHandle); break;
                default: break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix