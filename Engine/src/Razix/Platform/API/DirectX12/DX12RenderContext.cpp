// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12RenderContext.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Platform/API/DirectX12/DX12Context.h"
#include "Razix/Platform/API/DirectX12/DX12DrawCommandBuffer.h"
#include "Razix/Platform/API/DirectX12/DX12Fence.h"
#include "Razix/Platform/API/DirectX12/DX12Swapchain.h"
#include "Razix/Platform/API/DirectX12/DX12Texture.h"

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

            // Create the Draw/Compute Command buffers
            m_DrawCommandBuffers.set_capacity(MAX_SWAPCHAIN_BUFFERS);
            for (u32 i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
                auto commandBuffer = Graphics::RZDrawCommandBuffer::Create();
                commandBuffer->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Frame Draw Command Buffer: #" + std::to_string(i)));
                m_DrawCommandBuffers.push_back(commandBuffer);
            }

            // Cache the reference to the Vulkan context to avoid frequent calling
            m_Context = DX12Context::Get();
        }

        void DX12RenderContext::AcquireImageAPIImpl(RZSemaphore* signalSemaphore)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_CurrentCommandBuffer = m_DrawCommandBuffers.front();
            m_DrawCommandBuffers.pop_front();
        }

        void DX12RenderContext::SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores)
        {
        }

        void DX12RenderContext::BeginAPIImpl(RZDrawCommandBuffer* cmdBuffer)
        {
        }

        void DX12RenderContext::SubmitImpl(RZDrawCommandBuffer* cmdBuffer)
        {
        }

        void DX12RenderContext::PresentAPIImpl(RZSemaphore* waitSemaphore)
        {
        }

        void DX12RenderContext::BindPipelineImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer)
        {
        }

        void DX12RenderContext::BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx)
        {
        }

        void DX12RenderContext::BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets, u32 startSetIdx)
        {
        }

        void DX12RenderContext::BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets, u32 startSetIdx)
        {
        }

        void DX12RenderContext::EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer)
        {
        }

        void DX12RenderContext::BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors)
        {
        }

        void DX12RenderContext::BeginRenderingImpl(RZDrawCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo)
        {
        }

        void DX12RenderContext::EndRenderingImpl(RZDrawCommandBuffer* cmdBuffer)
        {
        }

        void DX12RenderContext::InsertImageMemoryBarrierImpl(RZDrawCommandBuffer* cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo)
        {
        }

        void DX12RenderContext::InsertBufferMemoryBarrierImpl(RZDrawCommandBuffer* cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo)
        {
        }

        void DX12RenderContext::CopyTextureResourceImpl(RZDrawCommandBuffer* cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle)
        {
        }

        void DX12RenderContext::DrawAPIImpl(RZDrawCommandBuffer* cmdBuffer, u32 count, DataType datayType /*= DataType::UNSIGNED_INT*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZEngine::Get().GetStatistics().NumDrawCalls++;
            RZEngine::Get().GetStatistics().Draws++;
            auto commandList = static_cast<DX12DrawCommandBuffer*>(cmdBuffer)->getD3DCommandList();
            commandList->DrawInstanced(count, 1, 0, 0);
        }

        void DX12RenderContext::DrawIndexedAPIImpl(RZDrawCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount /*= 1*/, u32 firstIndex /*= 0*/, int32_t vertexOffset /*= 0*/, u32 firstInstance /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZEngine::Get().GetStatistics().NumDrawCalls++;
            RZEngine::Get().GetStatistics().IndexedDraws++;
            auto commandList = static_cast<DX12DrawCommandBuffer*>(cmdBuffer)->getD3DCommandList();
            commandList->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        }

        void DX12RenderContext::DestroyAPIImpl()
        {
        }

        void DX12RenderContext::OnResizeAPIImpl(u32 width, u32 height)
        {
        }

        void DX12RenderContext::BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, RZPushConstant pushConstant)
        {
        }

        void DX12RenderContext::SetViewportImpl(RZDrawCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
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

            auto commandList = static_cast<DX12DrawCommandBuffer*>(cmdBuffer)->getD3DCommandList();
            commandList->RSSetViewports(1, &viewport);
        }

        void DX12RenderContext::SetScissorRectImpl(RZDrawCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(width != 0 || height != 0, "Scissor Rect Width or Height cannot be zero!");

            D3D12_RECT scissor = {};
            scissor.left       = x;
            scissor.top        = y;
            scissor.right      = width;
            scissor.bottom     = height;

            auto commandList = static_cast<DX12DrawCommandBuffer*>(cmdBuffer)->getD3DCommandList();
            commandList->RSSetScissorRects(1, &scissor);
        }

        void DX12RenderContext::SetDepthBiasImpl(RZDrawCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

        }

        Razix::Graphics::RZSwapchain* DX12RenderContext::GetSwapchainImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return static_cast<RZSwapchain*>(DX12Context::Get()->getSwapchain().get());
        }

    }    // namespace Graphics
}    // namespace Razix