// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKRenderContext.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"
#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "Razix/Platform/API/Vulkan/VKDescriptorSet.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKFence.h"
#include "Razix/Platform/API/Vulkan/VKPipeline.h"
#include "Razix/Platform/API/Vulkan/VKSemaphore.h"
#include "Razix/Platform/API/Vulkan/VKTexture.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Razix {
    namespace Graphics {

        static constexpr u32 MAX_DESCRIPTOR_SET_COUNT = 2500;

        static void CmdBeginRenderingKHR(VkCommandBuffer commandBuffer, const VkRenderingInfo* pRenderingInfo)
        {
            auto func = (PFN_vkCmdBeginRenderingKHR) vkGetDeviceProcAddr(VKDevice::Get().getDevice(), "vkCmdBeginRenderingKHR");
            if (func != nullptr)
                func(commandBuffer, pRenderingInfo);
            else
                RAZIX_CORE_ERROR("Function not found");
        }

        static void CmdEndRenderingKHR(VkCommandBuffer commandBuffer)
        {
            auto func = (PFN_vkCmdEndRenderingKHR) vkGetDeviceProcAddr(VKDevice::Get().getDevice(), "vkCmdEndRenderingKHR");
            if (func != nullptr)
                func(commandBuffer);
            else
                RAZIX_CORE_ERROR("Function not found");
        }

        VKRenderContext::VKRenderContext(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_RendererTitle = "Vulkan";
            m_Width         = width;
            m_Height        = height;
            m_PrevWidth     = width;
            m_PrevHeight    = height;

            // Create any extra descriptor pools here such as for ImGui and other needs
            std::array<VkDescriptorPoolSize, 5> pool_sizes = {
                VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLER, 100},
                VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100},
                VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100},
                VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100},
                VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100}};

            VkDescriptorPoolCreateInfo poolCreateInfo = {};
            poolCreateInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolCreateInfo.flags                      = 0;
            poolCreateInfo.poolSizeCount              = static_cast<u32>(pool_sizes.size());
            poolCreateInfo.pPoolSizes                 = pool_sizes.data();
            poolCreateInfo.maxSets                    = MAX_DESCRIPTOR_SET_COUNT;

            // allocate the all-in-on pool
            if (VK_CHECK_RESULT(vkCreateDescriptorPool(VKDevice::Get().getDevice(), &poolCreateInfo, nullptr, &m_DescriptorPool)))
                RAZIX_CORE_ERROR("[Vulkan] Cannot allocate descriptor pool by VKRenderer!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Successfully creates descriptor pool to allocate sets!");
        }

        VKRenderContext::~VKRenderContext()
        {
            //m_Context->Release();
        }

        void VKRenderContext::InitAPIImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Cache the reference to the Vulkan context to avoid frequent calling
            m_Context = VKContext::Get();
        }

        void VKRenderContext::AcquireImageAPIImpl(RZSemaphore* signalSemaphore)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto frameIdx = RHI::Get().GetSwapchain()->getCurrentImageIndex();

            // Get the next image to present
            m_Context->getSwapchain()->acquireNextImage(signalSemaphore ? *(VkSemaphore*) signalSemaphore->getHandle(frameIdx) : VK_NULL_HANDLE);
        }

        void VKRenderContext::BeginAPIImpl(RZCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Begin recording to the command buffer
            m_CurrentCommandBuffer = cmdBuffer;

            if (m_CurrentCommandBuffer->getState() == CommandBufferState::Submitted)
                m_Context->getSwapchain()->getCurrentFrameSyncData().renderFence->wait();

            cmdBuffer->BeginRecording();
        }

        void VKRenderContext::SubmitImpl(RZCommandBuffer* cmdBuffer)
        {
            // End the command buffer recording
            cmdBuffer->EndRecording();
            // Stack up the recorded command buffers for execution
            m_CommandQueue.push_back(cmdBuffer);
        }

        void VKRenderContext::SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto frameIdx     = RHI::Get().GetSwapchain()->getCurrentImageIndex();
            auto prevFrameIdx = frameIdx > 0 ? frameIdx - 1 : 2;

            std::vector<VkSemaphore> vkWaitSemaphores(waitSemaphores.size());
            for (sz i = 0; i < waitSemaphores.size(); i++)
                vkWaitSemaphores[i] = *(VkSemaphore*) waitSemaphores[i]->getHandle(prevFrameIdx);

            std::vector<VkSemaphore> vkSignalSemaphores(signalSemaphores.size());
            for (sz i = 0; i < signalSemaphores.size(); i++)
                vkSignalSemaphores[i] = *(VkSemaphore*) signalSemaphores[i]->getHandle(frameIdx);

            m_Context->getSwapchain()->queueSubmit(m_CommandQueue, vkWaitSemaphores, vkSignalSemaphores);

            m_CommandQueue.clear();
        }

        void VKRenderContext::PresentAPIImpl(RZSemaphore* waitSemaphore)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto frameIdx     = RHI::Get().GetSwapchain()->getCurrentImageIndex();
            auto prevFrameIdx = frameIdx > 0 ? frameIdx - 1 : 2;
            m_Context->getSwapchain()->present(waitSemaphore ? *(VkSemaphore*) waitSemaphore->getHandle(frameIdx) : VK_NULL_HANDLE);
        }

        void VKRenderContext::BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            u32 numDynamicDescriptorSets = 0;
            u32 numDesciptorSets         = 0;

            for (auto descriptorSet: descriptorSets) {
                if (descriptorSet) {
                    auto vkDescSet                        = static_cast<VKDescriptorSet*>(descriptorSet);
                    m_DescriptorSetPool[numDesciptorSets] = vkDescSet->getDescriptorSet();
                    numDesciptorSets++;
                }
            }
            vkCmdBindDescriptorSets(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VKPipeline*>(pipeline)->getPipelineLayout(), 0, numDesciptorSets, m_DescriptorSetPool, numDynamicDescriptorSets, nullptr);
        }

        void VKRenderContext::BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZDescriptorSet** descriptorSets, u32 totalSets)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            u32 numDynamicDescriptorSets = 0;
            u32 numDesciptorSets         = 0;

            for (u32 i = 0; i < totalSets; i++) {
                auto set = descriptorSets[i];
                if (set) {
                    auto vkDescSet                        = static_cast<VKDescriptorSet*>(set);
                    m_DescriptorSetPool[numDesciptorSets] = vkDescSet->getDescriptorSet();
                    numDesciptorSets++;
                }
            }
            vkCmdBindDescriptorSets(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VKPipeline*>(pipeline)->getPipelineLayout(), 0, numDesciptorSets, m_DescriptorSetPool, numDynamicDescriptorSets, nullptr);
        }

        void VKRenderContext::SetScissorRectImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VkRect2D scissorRect{};
            scissorRect.offset.x      = x;
            scissorRect.offset.y      = y;
            scissorRect.extent.width  = width;
            scissorRect.extent.height = height;

            vkCmdSetScissor(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), 0, 1, &scissorRect);
        }

        void VKRenderContext::BeginRenderingImpl(RZCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo)
        {
            VkRenderingInfoKHR renderingInfoKHR{};
            renderingInfoKHR.sType             = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
            renderingInfoKHR.renderArea.offset = {0, 0};
            renderingInfoKHR.renderArea.extent = {renderingInfo.extent.x, renderingInfo.extent.y};
            renderingInfoKHR.layerCount        = renderingInfo.layerCount;

            std::vector<VkRenderingAttachmentInfo> colorAttachments;

            for (auto& attachment: renderingInfo.colorAttachments) {
                // Resize attachments when resized
                if (renderingInfo.resize) {
                    if (m_Width != attachment.first->getWidth() || m_Height != attachment.first->getHeight())
                        attachment.first->Resize(renderingInfo.extent.x, renderingInfo.extent.y RZ_DEBUG_NAME_TAG_STR_E_ARG(attachment.first->getName()));
                }

                // Fill the color attachments first
                VkRenderingAttachmentInfoKHR attachInfo{};
                attachInfo.sType     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
                auto apiHandle       = static_cast<VkDescriptorImageInfo*>(attachment.first->GetHandle());
                attachInfo.imageView = apiHandle->imageView;

                if (attachment.first->getFormat() == RZTexture::Format::SCREEN) {
                    auto vkImage = static_cast<VKTexture2D*>(attachment.first);
                    VKUtilities::TransitionImageLayout(vkImage->getImage(), VKUtilities::TextureFormatToVK(vkImage->getFormat()), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL);
                    vkImage->setImageLayout(VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL);
                    attachInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                } else
                    attachInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;

                if (attachment.second.clear) {
                    attachInfo.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    attachInfo.storeOp = VK_ATTACHMENT_STORE_OP_NONE;
                } else {
                    attachInfo.loadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;    // Well don't discard stuff we render on top of what was presented previously
                    attachInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                }

                if (attachment.first->getType() == RZTexture::Type::COLOR_2D || attachment.first->getType() == RZTexture::Type::COLOR_3D || attachment.first->getType() == RZTexture::Type::COLOR_RT || attachment.first->getType() == RZTexture::Type::CUBEMAP) {
                    auto& clearColor = attachment.second.clearColor;
                    memcpy(attachInfo.clearValue.color.float32, &clearColor[0], sizeof(glm::vec4));
                    colorAttachments.push_back(attachInfo);
                } else if (attachment.first->getType() == RZTexture::Type::DEPTH) {
                    attachInfo.clearValue.depthStencil = VkClearDepthStencilValue{1.0f, 0};
                    renderingInfoKHR.pDepthAttachment  = &attachInfo;
                }
            }

            renderingInfoKHR.colorAttachmentCount = static_cast<u32>(colorAttachments.size());
            renderingInfoKHR.pColorAttachments    = colorAttachments.data();

            // Depth Attachment
            VkRenderingAttachmentInfoKHR attachInfo{};
            if (renderingInfo.depthAttachment.first)
            {
                // Depth attachment resize
                if (renderingInfo.resize) {
                    if (m_Width != renderingInfo.depthAttachment.first->getWidth() || m_Height != renderingInfo.depthAttachment.first->getHeight())
                        renderingInfo.depthAttachment.first->Resize(renderingInfo.extent.x, renderingInfo.extent.y RZ_DEBUG_NAME_TAG_STR_E_ARG(renderingInfo.depthAttachment.first->getName()));
                }

                // Fill the color attachments first
                attachInfo.sType                               = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
                VkDescriptorImageInfo* apiHandle = (VkDescriptorImageInfo*) (renderingInfo.depthAttachment.first->GetHandle());
                attachInfo.imageView                           = apiHandle->imageView;

                attachInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

                bool clearDepth = renderingInfo.depthAttachment.second.clear;

                if (clearDepth) {
                    attachInfo.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    attachInfo.storeOp = VK_ATTACHMENT_STORE_OP_NONE;
                } else {
                    attachInfo.loadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;    // Well don't discard stuff we render on top of what was presented previously
                    attachInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                }
                attachInfo.clearValue.depthStencil = VkClearDepthStencilValue{1.0f, 0};
                renderingInfoKHR.pDepthAttachment  = &attachInfo;
            }

            CmdBeginRenderingKHR(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), &renderingInfoKHR);
        }

        void VKRenderContext::EndRenderingImpl(RZCommandBuffer* cmdBuffer)
        {
            CmdEndRenderingKHR(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer());
        }

        void VKRenderContext::SetCmdCheckpointImpl(RZCommandBuffer* cmdbuffer, void* markerData)
        {
#if 0
            RAZIX_CORE_WARN("Marker Data set : {0} at memory location : {1}", *static_cast<std::string*>(markerData), markerData);

            auto func = (PFN_vkCmdSetCheckpointNV) vkGetDeviceProcAddr(VKDevice::Get().getDevice(), "vkCmdSetCheckpointNV");
            if (func != nullptr)
                func(static_cast<VKCommandBuffer*>(cmdbuffer)->getBuffer(), markerData);
#endif
        }

        void VKRenderContext::DrawAPIImpl(RZCommandBuffer* cmdBuffer, u32 count, DataType datayType /*= DataType::UNSIGNED_INT*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZEngine::Get().GetStatistics().NumDrawCalls++;
            RZEngine::Get().GetStatistics().Draws++;
            vkCmdDraw(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), count, 1, 0, 0);
        }

        void VKRenderContext::DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, int32_t vertexOffset, u32 firstInstance)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZEngine::Get().GetStatistics().NumDrawCalls++;
            RZEngine::Get().GetStatistics().IndexedDraws++;
            vkCmdDrawIndexed(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        }

        void VKRenderContext::DestroyAPIImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Destroy the descriptor pool
            vkDestroyDescriptorPool(VKDevice::Get().getDevice(), m_DescriptorPool, nullptr);
        }

        void VKRenderContext::OnResizeAPIImpl(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Width  = width;
            m_Height = height;

            vkDeviceWaitIdle(VKDevice::Get().getDevice());
            m_Context->getSwapchain().get()->OnResize(width, height);
        }

        RZSwapchain* VKRenderContext::GetSwapchainImpl()
        {
            return static_cast<RZSwapchain*>(VKContext::Get()->getSwapchain().get());
        }

        void VKRenderContext::BindPushConstantsAPIImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            //for (auto& pushConstant: pushConstants) {
            vkCmdPushConstants(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), static_cast<VKPipeline*>(pipeline)->getPipelineLayout(), VKUtilities::ShaderStageToVK(pushConstant.shaderStage), pushConstant.offset, pushConstant.size, pushConstant.data);
            //}
        }

        void VKRenderContext::SetDepthBiasImpl(RZCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            f32 depthBiasConstant = 1.25f;
            // Slope depth bias factor, applied depending on polygon's slope
            f32 depthBiasSlope = 1.75f;
            vkCmdSetDepthBias(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), depthBiasConstant, 0.0f, depthBiasSlope);
        }
    }    // namespace Graphics
}    // namespace Razix
