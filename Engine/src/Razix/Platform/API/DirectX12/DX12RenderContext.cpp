// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12RenderContext.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12CommandPool.h"
    #include "Razix/Platform/API/DirectX12/DX12Context.h"
    #include "Razix/Platform/API/DirectX12/DX12DrawCommandBuffer.h"
    #include "Razix/Platform/API/DirectX12/DX12Fence.h"
    #include "Razix/Platform/API/DirectX12/DX12Pipeline.h"
    #include "Razix/Platform/API/DirectX12/DX12RootSignature.h"
    #include "Razix/Platform/API/DirectX12/DX12Swapchain.h"
    #include "Razix/Platform/API/DirectX12/DX12Texture.h"
    #include "Razix/Platform/API/DirectX12/DX12Utilities.h"

    #include <imgui/imgui.h>

namespace Razix {
    namespace Graphics {

        DX12RenderContext::DX12RenderContext(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_RendererTitle = "DirectX12";
            m_Width         = width;
            m_Height        = height;
            m_PrevWidth     = width;
            m_PrevHeight    = height;
        }

        //--------------------------------------------------------------------------------------

        void DX12RenderContext::OnImGui()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (ImGui::Begin("RHI [D3D12]")) {
                ImGui::Text("Welcome to DirectX12 rendering backend.");
            }
            ImGui::End();
        }

        //--------------------------------------------------------------------------------------

        void DX12RenderContext::InitAPIImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

    #ifndef RAZIX_DISTRIBUTION
            DX12Utilities::LoadPIXRuntime();
    #endif

            // Create the Draw/Compute Command buffers
            m_DrawCommandBuffers.set_capacity(MAX_SWAPCHAIN_BUFFERS);
            m_GraphicsCommandPool.set_capacity(MAX_SWAPCHAIN_BUFFERS);

            for (u32 i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
                auto pool                  = RZResourceManager::Get().createCommandPool(PoolType::kGraphics);
                m_GraphicsCommandPool[i]   = pool;
                m_DrawCommandBuffers[i]    = RZResourceManager::Get().createDrawCommandBuffer(pool);
                auto commandBufferResource = RZResourceManager::Get().getDrawCommandBufferResource(m_DrawCommandBuffers[i]);
                commandBufferResource->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Frame Draw Command Buffer: #" + std::to_string(i)));
            }

            // Cache the reference to the Vulkan context to avoid frequent calling
            m_Context = DX12Context::Get();

            m_RenderReadyFence = rzstl::CreateUniqueRef<DX12Fence>();
        }

        void DX12RenderContext::AcquireImageAPIImpl(RZSemaphore* signalSemaphore)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_CurrentCommandBuffer = m_DrawCommandBuffers[RHI::Get().GetSwapchain()->getCurrentFrameIndex()];

            // Wait until previous rendering is finished for the current acquired command buffer
            m_RenderReadyFence->wait(m_InflightFramesFenceValues[RHI::Get().GetSwapchain()->getCurrentFrameIndex()]);

            // Acquire the Swapchain back buffer
            m_Context->getSwapchain()->acquireBackBuffer();
        }

        void DX12RenderContext::BeginAPIImpl(RZDrawCommandBufferHandle cmdBuffer)
        {
            m_CurrentCommandBuffer = cmdBuffer;

            auto commandBufferResource = RZResourceManager::Get().getDrawCommandBufferResource(m_CurrentCommandBuffer);
            // Reset the Command Allocator and Command List and begin recording commands
            commandBufferResource->BeginRecording();

            // Prepare swapchain for rendering stuff onto
            auto commandListD3D = (ID3D12GraphicsCommandList2*) RZResourceManager::Get().getDrawCommandBufferResource(cmdBuffer)->getAPIBuffer();
            DX12Utilities::TransitionResource(commandListD3D, DX12Context::Get()->getSwapchain()->getCurrentD3DBackbufferResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        }

        RAZIX_DEPRECATED("[Razix Deprecated!] SubmitWork is no longer used, use RHI::Submit(RZDrawCommandBuffer*) to submit draw commands & execute work on CPU.")
        void DX12RenderContext::SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX12RenderContext::SubmitImpl(RZDrawCommandBufferHandle cmdBuffer)
        {
            auto commandBufferResource = RZResourceManager::Get().getDrawCommandBufferResource(cmdBuffer);

            // Ready the swapchain image from Render TArget stat to Present state for the presentation engine
            auto commandListD3D = (ID3D12GraphicsCommandList2*) commandBufferResource->getAPIBuffer();
            // Now that we hare rendered onto it convert it back to presentable
            DX12Utilities::TransitionResource(commandListD3D, DX12Context::Get()->getSwapchain()->getCurrentD3DBackbufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

            commandBufferResource->EndRecording();
            // Stack up the recorded command buffers for execution
            m_GraphicsCommandQueue.push_back(cmdBuffer);

            commandBufferResource->Execute();
        }

        void DX12RenderContext::PresentAPIImpl(RZSemaphore* waitSemaphore)
        {
            m_Context->getSwapchain()->present();

            // Signal the fence
            u64 fenceValueForNextWait                                                      = m_RenderReadyFence->signal(DX12Context::Get()->getGraphicsQueue());
            m_InflightFramesFenceValues[RHI::Get().GetSwapchain()->getCurrentFrameIndex()] = fenceValueForNextWait;

            m_GraphicsCommandQueue.clear();
        }

        void DX12RenderContext::BindPipelineImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer)
        {
            auto pp = RZResourceManager::Get().getPool<RZPipeline>().get(pipeline);
            pp->Bind(cmdBuffer);
        }

        void DX12RenderContext::BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX12RenderContext::BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets, u32 startSetIdx)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX12RenderContext::BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets, u32 startSetIdx)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX12RenderContext::EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX12RenderContext::BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptor>& descriptors)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX12RenderContext::BeginRenderingImpl(RZDrawCommandBufferHandle cmdBuffer, const RenderingInfo& renderingInfo)
        {
            //----------------------------------------------
            // TESTING HELLO_TRIANGLE

            // TODO: Clear the RTs and DTs + bind the actual RT textures to render onto and set the viewport etc

            glm::ivec2 viewPortExtents;
            if (renderingInfo.resolution == Resolution::kCustom)
                viewPortExtents = {renderingInfo.extent.x, renderingInfo.extent.y};
            else if (renderingInfo.resolution == Resolution::kWindow)
                viewPortExtents = {m_Width, m_Height};
            else {
                auto& res       = ResolutionToExtentsMap[renderingInfo.resolution];
                viewPortExtents = {res.x, res.y};
            }

            SetViewport(cmdBuffer, 0, 0, viewPortExtents.x, viewPortExtents.y);
            SetScissorRect(cmdBuffer, 0, 0, viewPortExtents.x, viewPortExtents.y);

            // Set the RT/DT
            //if (renderingInfo.colorAttachments.Handle == SWAPCHAIN[frameIndex].Handle)
            auto rtv = m_Context->getSwapchain()->getCurrentBackBufferRTVHandle();

            auto commandBufferResource = RZResourceManager::Get().getDrawCommandBufferResource(cmdBuffer);
            auto commandListD3D        = (ID3D12GraphicsCommandList2*) commandBufferResource->getAPIBuffer();
            commandListD3D->OMSetRenderTargets(1, &rtv, false, nullptr);
            //----------------------------------------------
        }

        void DX12RenderContext::EndRenderingImpl(RZDrawCommandBufferHandle cmdBuffer)
        {
        }

        void DX12RenderContext::InsertImageMemoryBarrierImpl(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX12RenderContext::InsertBufferMemoryBarrierImpl(RZDrawCommandBufferHandle cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX12RenderContext::CopyTextureResourceImpl(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX12RenderContext::DrawAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 count, DataType datayType /*= DataType::UNSIGNED_INT*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZEngine::Get().GetStatistics().NumDrawCalls++;
            RZEngine::Get().GetStatistics().Draws++;
            auto cmdBufferResource = RZResourceManager::Get().getDrawCommandBufferResource(cmdBuffer);
            auto commandList       = static_cast<DX12DrawCommandBuffer*>(cmdBufferResource)->getD3DCommandList();
            commandList->DrawInstanced(count, 1, 0, 0);
        }

        void DX12RenderContext::DrawIndexedAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 indexCount, u32 instanceCount /*= 1*/, u32 firstIndex /*= 0*/, int32_t vertexOffset /*= 0*/, u32 firstInstance /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZEngine::Get().GetStatistics().NumDrawCalls++;
            RZEngine::Get().GetStatistics().IndexedDraws++;
            auto cmdBufferResource = RZResourceManager::Get().getDrawCommandBufferResource(cmdBuffer);
            auto commandList       = static_cast<DX12DrawCommandBuffer*>(cmdBufferResource)->getD3DCommandList();
            commandList->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        }

        void DX12RenderContext::DispatchAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 groupX, u32 groupY, u32 groupZ)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // FIXME: Don't we need a Compute/Async Command List?
            RZEngine::Get().GetStatistics().ComputeDispatches++;
            auto cmdBufferResource = RZResourceManager::Get().getDrawCommandBufferResource(cmdBuffer);
            auto commandList       = static_cast<DX12DrawCommandBuffer*>(cmdBufferResource)->getD3DCommandList();
            commandList->Dispatch(groupX, groupY, groupZ);
        }

        void DX12RenderContext::DestroyAPIImpl()
        {
            DX12Context::Get()->getSwapchain()->Destroy();
        }

        void DX12RenderContext::OnResizeAPIImpl(u32 width, u32 height)
        {
            m_Width  = width;
            m_Height = height;

            // Flush any and all queues including graphics and compute
            m_RenderReadyFence->flush(DX12Context::Get()->getGraphicsQueue());

            m_Context->getSwapchain().get()->OnResize(width, height);
        }

        void DX12RenderContext::BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZPushConstant pushConstant)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX12RenderContext::SetViewportImpl(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(width != 0 || height != 0, "Viewport Width or Height cannot be zero!");

            D3D12_VIEWPORT viewport = {};
            viewport.TopLeftX       = static_cast<f32>(x);
            viewport.TopLeftY       = static_cast<f32>(y);
            viewport.Width          = static_cast<f32>(width);
            viewport.Height         = static_cast<f32>(height);
            viewport.MinDepth       = 0.0f;
            viewport.MaxDepth       = 1.0f;

            auto cmdBufferResource = RZResourceManager::Get().getDrawCommandBufferResource(cmdBuffer);
            auto commandList       = static_cast<DX12DrawCommandBuffer*>(cmdBufferResource)->getD3DCommandList();
            commandList->RSSetViewports(1, &viewport);
        }

        void DX12RenderContext::SetScissorRectImpl(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(width != 0 || height != 0, "Scissor Rect Width or Height cannot be zero!");

            D3D12_RECT scissor = {};
            scissor.left       = x;
            scissor.top        = y;
            scissor.right      = width;
            scissor.bottom     = height;

            auto cmdBufferResource = RZResourceManager::Get().getDrawCommandBufferResource(cmdBuffer);
            auto commandList       = static_cast<DX12DrawCommandBuffer*>(cmdBufferResource)->getD3DCommandList();
            commandList->RSSetScissorRects(1, &scissor);
        }

        void DX12RenderContext::SetDepthBiasImpl(RZDrawCommandBufferHandle cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_UNIMPLEMENTED_METHOD
        }

        Razix::Graphics::RZSwapchain* DX12RenderContext::GetSwapchainImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return static_cast<RZSwapchain*>(DX12Context::Get()->getSwapchain().get());
        }
    }    // namespace Graphics
}    // namespace Razix

#endif