// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12RenderContext.h"

#include "Razix/Platform/API/DirectX12/DX12Context.h"

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

            // Cache the reference to the Vulkan context to avoid frequent calling
            m_Context = DX12Context::Get();
        }

        void DX12RenderContext::AcquireImageAPIImpl(RZSemaphore* signalSemaphore)
        {
        }

        void DX12RenderContext::SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores)
        {
        }

        void DX12RenderContext::BeginAPIImpl(RZCommandBuffer* cmdBuffer)
        {
        }

        void DX12RenderContext::SubmitImpl(RZCommandBuffer* cmdBuffer)
        {
        }

        void DX12RenderContext::PresentAPIImpl(RZSemaphore* waitSemaphore)
        {
        }

        void DX12RenderContext::BindPipelineImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer)
        {
        }

        void DX12RenderContext::BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx)
        {
        }

        void DX12RenderContext::BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets, u32 startSetIdx)
        {
        }

        void DX12RenderContext::BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets, u32 startSetIdx)
        {
        }

        void DX12RenderContext::SetScissorRectImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
        {
        }

        void DX12RenderContext::EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer)
        {
        }

        void DX12RenderContext::BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors)
        {
        }

        void DX12RenderContext::BeginRenderingImpl(RZCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo)
        {
        }

        void DX12RenderContext::EndRenderingImpl(RZCommandBuffer* cmdBuffer)
        {
        }

        void DX12RenderContext::InsertImageMemoryBarrierImpl(RZCommandBuffer* cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo)
        {
        }

        void DX12RenderContext::InsertBufferMemoryBarrierImpl(RZCommandBuffer* cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo)
        {
        }

        void DX12RenderContext::CopyTextureResourceImpl(RZCommandBuffer* cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle)
        {
        }

        void DX12RenderContext::DrawAPIImpl(RZCommandBuffer* cmdBuffer, u32 count, DataType datayType /*= DataType::UNSIGNED_INT*/)
        {
        }

        void DX12RenderContext::DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount /*= 1*/, u32 firstIndex /*= 0*/, int32_t vertexOffset /*= 0*/, u32 firstInstance /*= 0*/)
        {
        }

        void DX12RenderContext::DestroyAPIImpl()
        {
        }

        void DX12RenderContext::OnResizeAPIImpl(u32 width, u32 height)
        {
        }

        void DX12RenderContext::BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant)
        {
        }

        void DX12RenderContext::SetViewportImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
        {
        }

        void DX12RenderContext::SetDepthBiasImpl(RZCommandBuffer* cmdBuffer)
        {
        }

        Razix::Graphics::RZSwapchain* DX12RenderContext::GetSwapchainImpl()
        {
            return nullptr;
        }

    }    // namespace Graphics
}    // namespace Razix