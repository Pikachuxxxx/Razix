// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RHI.h"

#include "Razix/Core/App/RZApplication.h"

#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKRenderContext.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include "Razix/Platform/API/DirectX12/DX12RenderContext.h"
#endif

namespace Razix {
    namespace Gfx {

        //------------------------------------
        RHI* RHI::s_APIInstance = nullptr;
        //------------------------------------

        //------------------------------------------------------------------------
        // Lifecycle Management
        void RHI::Create(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_CORE_INFO("[RHI]Creating API Renderer...");
            RAZIX_CORE_INFO("[RHI]\t Backend API : {0}", Gfx::RZGraphicsContext::GetRenderAPIString());

            switch (Gfx::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Gfx::RenderAPI::VULKAN: s_APIInstance = new VKRenderContext(width, height); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Gfx::RenderAPI::D3D12: s_APIInstance = new DX12RenderContext(width, height); break;
#endif
                default: s_APIInstance = nullptr; break;
            }
        }

        void RHI::Destroy()
        {
            // destroy frame data and light descriptor sets
            if (s_APIInstance->m_FrameDataSet.isValid())
                RZResourceManager::Get().destroyDescriptorSet(s_APIInstance->m_FrameDataSet);
            if (s_APIInstance->m_SceneLightsDataSet.isValid())
                RZResourceManager::Get().destroyDescriptorSet(s_APIInstance->m_SceneLightsDataSet);

            // Destroy Swapchain back buffer resources first
            GetSwapchain()->DestroyBackBufferImages();

            // Destroy the backend renderer resources
            s_APIInstance->DestroyAPIImpl();
            delete s_APIInstance;

            // Release the context at last
            Gfx::RZGraphicsContext::GetContext()->Destroy();
        }

        RHI& RHI::Get()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            return *s_APIInstance;
        }

        const RHI* RHI::GetPointer()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            return s_APIInstance;
        }

        void RHI::Init()
        {
            RAZIX_PROFILE_GPU_SCOPE("Init Rendering");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->InitAPIImpl();
        }

        void RHI::OnResize(u32 width, u32 height)
        {
            RAZIX_PROFILE_GPU_SCOPE("OnResize");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->OnResizeAPIImpl(width, height);
        }

        //------------------------------------------------------------------------
        // Command Recording & Submission
        void RHI::AcquireImage(RZSemaphore* signalSemaphore)
        {
            RAZIX_PROFILE_GPU_SCOPE("Acquire swap image");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->AcquireImageAPIImpl(signalSemaphore);
        }
        void RHI::Begin(RZDrawCommandBufferHandle cmdBuffer)
        {
            RAZIX_PROFILE_GPU_SCOPE("Begin Rendering");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->BeginAPIImpl(cmdBuffer);
        }
        void RHI::Submit(RZDrawCommandBufferHandle cmdBuffer)
        {
            RAZIX_PROFILE_GPU_SCOPE("Submit To Render Queue");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->SubmitImpl(cmdBuffer);
        }

        void RHI::Present(RZSemaphore* waitSemaphore)
        {
            RAZIX_PROFILE_GPU_SCOPE("Present Swapchain");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->PresentAPIImpl(waitSemaphore);
        }

        //------------------------------------------------------------------------
        // Binding
        void RHI::BindPipeline(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer)
        {
            RAZIX_PROFILE_GPU_SCOPE("Bind Pipeline");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->BindPipelineImpl(pipeline, cmdBuffer);
        }
        void RHI::BindDescriptorSet(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZDescriptorSetHandle descriptorSet, u32 setIdx)
        {
            RAZIX_PROFILE_GPU_SCOPE("Bind Descriptor Set at Idx");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->BindDescriptorSetAPImpl(pipeline, cmdBuffer, descriptorSet, setIdx);
        }
        void RHI::BindUserDescriptorSets(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptorSetHandle>& descriptorSets, u32 startSetIdx)
        {
            RAZIX_PROFILE_GPU_SCOPE("Bind Descriptor Sets");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->BindUserDescriptorSetsAPImpl(pipeline, cmdBuffer, descriptorSets, startSetIdx);
        }
        void RHI::BindPushConstant(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZPushConstant pushConstant)
        {
            RAZIX_PROFILE_GPU_SCOPE("Bind Pushconstants");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->BindPushConstantsAPIImpl(pipeline, cmdBuffer, pushConstant);
        }

        void RHI::EnableBindlessTextures(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer)
        {
            RAZIX_PROFILE_GPU_SCOPE("Enable Bindless Textures");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->EnableBindlessTexturesImpl(pipeline, cmdBuffer);
        }

        //------------------------------------------------------------------------
        // Draws & Dispatches

        void RHI::Draw(RZDrawCommandBufferHandle cmdBuffer, u32 count, DrawDataType dataType)
        {
            RAZIX_PROFILE_GPU_SCOPE("Draw");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->DrawAPIImpl(cmdBuffer, count, dataType);
        }
        void RHI::DrawIndexed(RZDrawCommandBufferHandle cmdBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, int32_t vertexOffset, u32 firstInstance)
        {
            RAZIX_PROFILE_GPU_SCOPE("Draw Indexed");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->DrawIndexedAPIImpl(cmdBuffer, indexCount);
        }
        void RHI::Dispatch(RZDrawCommandBufferHandle cmdBuffer, u32 groupX, u32 groupY, u32 groupZ)
        {
            RAZIX_PROFILE_GPU_SCOPE("Dispatch");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(groupX || groupY || groupZ, "[Vulkan Render Context] either of groupX/groupY/groupZ should be non-zero.");

            s_APIInstance->DispatchAPIImpl(cmdBuffer, groupX, groupY, groupZ);
        }

        //------------------------------------------------------------------------
        // Render Pass
        void RHI::BeginRendering(RZDrawCommandBufferHandle cmdBuffer, const RenderingInfo& renderingInfo)
        {
            RAZIX_PROFILE_GPU_SCOPE("Begin Rendering");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->BeginRenderingImpl(cmdBuffer, renderingInfo);
        }
        void RHI::EndRendering(RZDrawCommandBufferHandle cmdBuffer)
        {
            RAZIX_PROFILE_GPU_SCOPE("End Rendering");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->EndRenderingImpl(cmdBuffer);
        }

        //------------------------------------------------------------------------
        // Memory Barriers
        void RHI::InsertImageMemoryBarrier(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo)
        {
            RAZIX_PROFILE_GPU_SCOPE("Insert Image Memory Barrier");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            s_APIInstance->InsertImageMemoryBarrierImpl(cmdBuffer, texture, pipelineBarrierInfo, imgBarrierInfo);
        }
        void RHI::InsertBufferMemoryBarrier(RZDrawCommandBufferHandle cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo)
        {
            RAZIX_PROFILE_GPU_SCOPE("Insert Buffer Memory Barrier");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_APIInstance->InsertBufferMemoryBarrierImpl(cmdBuffer, buffer, pipelineBarrierInfo, bufBarrierInfo);
        }

        //------------------------------------------------------------------------
        // Resource Management
        void RHI::CopyTextureResource(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle)
        {
            RAZIX_PROFILE_GPU_SCOPE("Copy Texture Resource");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return s_APIInstance->CopyTextureResourceImpl(cmdBuffer, dstTexture, srcTextureHandle);
        }

        Razix::Gfx::TextureReadback RHI::InsertTextureReadback(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle texture)
        {
            RAZIX_PROFILE_GPU_SCOPE("Texture Readback");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return s_APIInstance->InsertTextureReadbackImpl(cmdBuffer, texture);
        }

        //------------------------------------------------------------------------
        // Pipeline
        void RHI::SetDepthBias(RZDrawCommandBufferHandle cmdBuffer)
        {
            RAZIX_PROFILE_GPU_SCOPE("Set Depth Bias");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return s_APIInstance->SetDepthBiasImpl(cmdBuffer);
        }
        void RHI::SetViewport(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
        {
            RAZIX_PROFILE_GPU_SCOPE("SetViewport");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return s_APIInstance->SetViewportImpl(cmdBuffer, x, y, width, height);
        }
        void RHI::SetScissorRect(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
        {
            RAZIX_PROFILE_GPU_SCOPE("SetScissorRect");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return s_APIInstance->SetScissorRectImpl(cmdBuffer, x, y, width, height);
        }

        //------------------------------------------------------------------------
        // Misc
        RZSwapchain* RHI::GetSwapchain()
        {
            RAZIX_PROFILE_GPU_SCOPE("GetSwapchain");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return s_APIInstance->GetSwapchainImpl();
        }
        RZDrawCommandBufferHandle RHI::GetCurrentCommandBuffer()
        {
            RAZIX_PROFILE_GPU_SCOPE("GetCurrentCommandBuffer");
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return s_APIInstance->m_CurrentCommandBuffer;
        }
    }    // namespace Gfx
}    // namespace Razix
