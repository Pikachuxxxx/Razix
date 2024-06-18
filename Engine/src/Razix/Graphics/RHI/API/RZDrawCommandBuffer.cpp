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

namespace Razix {
    namespace Graphics {

        RZDrawCommandBuffer* RZDrawCommandBuffer::BeginSingleTimeCommandBuffer()
        {
            auto                 vkCmdBuffer = VKUtilities::BeginSingleTimeCommandBuffer();
            VKDrawCommandBuffer* cmdBuffer   = new VKDrawCommandBuffer(vkCmdBuffer);
            return (RZDrawCommandBuffer*) cmdBuffer;
        }

        void RZDrawCommandBuffer::EndSingleTimeCommandBuffer(RZDrawCommandBuffer* cmdBuffer)
        {
            return VKUtilities::EndSingleTimeCommandBuffer(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer());
        }

        RZDrawCommandBuffer* RZDrawCommandBuffer::Create(RZCommandAllocatorPool* pool)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLCommandBuffer(); break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKDrawCommandBuffer(); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix