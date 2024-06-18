// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKRenderContext.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"
#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "Razix/Platform/API/Vulkan/VKDescriptorSet.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKDrawCommandBuffer.h"
#include "Razix/Platform/API/Vulkan/VKPipeline.h"
#include "Razix/Platform/API/Vulkan/VKSemaphore.h"
#include "Razix/Platform/API/Vulkan/VKTexture.h"
#include "Razix/Platform/API/Vulkan/VKUniformBuffer.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

namespace Razix {
    namespace Graphics {

        static constexpr u32 MAX_DESCRIPTOR_SET_COUNT = 2500;

        static void CmdBeginRenderingKHR(VkCommandBuffer commandBuffer, const VkRenderingInfo* pRenderingInfo)
        {
            auto func = (PFN_vkCmdBeginRenderingKHR) vkGetDeviceProcAddr(VKDevice::Get().getDevice(), "vkCmdBeginRenderingKHR");
            if (func != nullptr)
                func(commandBuffer, pRenderingInfo);
            else
                RAZIX_CORE_ERROR("VkCmdBeginRenderingKHR Function not found");
        }

        static void CmdEndRenderingKHR(VkCommandBuffer commandBuffer)
        {
            auto func = (PFN_vkCmdEndRenderingKHR) vkGetDeviceProcAddr(VKDevice::Get().getDevice(), "vkCmdEndRenderingKHR");
            if (func != nullptr)
                func(commandBuffer);
            else
                RAZIX_CORE_ERROR("VkCmdEndRenderingKHR Function not found");
        }

        static void CmdPushDescriptorSetKHR(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites)
        {
            auto func = (PFN_vkCmdPushDescriptorSetKHR) vkGetDeviceProcAddr(VKDevice::Get().getDevice(), "vkCmdPushDescriptorSetKHR");
            if (func != nullptr)
                func(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
            else
                RAZIX_CORE_ERROR("vkCmdPushDescriptorSetKHR Function not found");
        }

        //--------------------------------------------------------------------------------------

        VKRenderContext::VKRenderContext(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_RendererTitle = "Vulkan";
            m_Width         = width;
            m_Height        = height;
            m_PrevWidth     = width;
            m_PrevHeight    = height;
        }

        void VKRenderContext::OnImGui()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (ImGui::Begin("RHI [Vulkan]")) {
                auto m_PhysicalDeviceProperties = VKDevice::Get().getPhysicalDevice()->getProperties();

                ImGui::Text("Vulkan API Version : %d.%d.%d", VK_VERSION_MAJOR(m_PhysicalDeviceProperties.apiVersion), VK_VERSION_MINOR(m_PhysicalDeviceProperties.apiVersion), VK_VERSION_PATCH(m_PhysicalDeviceProperties.apiVersion));
                ImGui::Text("GPU Name           : %s", std::string(m_PhysicalDeviceProperties.deviceName).c_str());
                ImGui::Text("Vendor ID          : %s", std::to_string(m_PhysicalDeviceProperties.vendorID).c_str());
                ImGui::Text("Device Type        : %s", std::string(VKDevice::Get().getPhysicalDevice()->getPhysicalDeviceTypeString(m_PhysicalDeviceProperties.deviceType)).c_str());
                ImGui::Text("Driver Version     : %d.%d.%d", VK_VERSION_MAJOR(m_PhysicalDeviceProperties.driverVersion), VK_VERSION_MINOR(m_PhysicalDeviceProperties.driverVersion), VK_VERSION_PATCH(m_PhysicalDeviceProperties.driverVersion));

                ImGui::Separator();

                auto gpuMemProps = VKDevice::Get().getPhysicalDevice()->getMemoryProperties();

                VmaBudget* pBudgets = new VmaBudget[gpuMemProps.memoryHeapCount];
                vmaGetHeapBudgets(VKDevice::Get().getVMA(), pBudgets);

                ImGui::Text("Heaps Count : %d", gpuMemProps.memoryHeapCount);

                for (u32 i = 0; i < gpuMemProps.memoryHeapCount; i++) {
                    auto heapName = "Heap " + std::to_string(i);
                    if (ImGui::CollapsingHeader(heapName.c_str())) {
                        ImGui::Text("Idx              : %d", i);
                        ImGui::Text("Memory Flags     : %d", gpuMemProps.memoryHeaps[i].flags);
                        ImGui::Text("Heap Size        : %f Gib", in_Gib((float) gpuMemProps.memoryHeaps[i].size));
                        ImGui::Text("Heap Usage       : %f Gib", in_Gib((float) pBudgets[i].usage));
                        ImGui::Text("Heap Budget      : %f Gib", in_Gib((float) pBudgets[i].budget));
                        ImGui::Text("Block Count      : %d", pBudgets[i].statistics.blockCount);
                        ImGui::Text("Allocation Count : %d", pBudgets[i].statistics.allocationCount);
                        ImGui::Text("Block Bytes      : %f Gib", in_Gib((float) pBudgets[i].statistics.blockBytes));
                        ImGui::Text("Allocation Bytes : %f Gib", in_Gib((float) pBudgets[i].statistics.allocationBytes));
                    }
                }
                ImGui::Separator();

                if (ImGui::Button("Calculate full statistics")) {
                    VmaTotalStatistics totalStats{};
                    vmaCalculateStatistics(VKDevice::Get().getVMA(), &totalStats);
                }
            }
            ImGui::End();
        }

        void VKRenderContext::InitAPIImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_DrawCommandBuffers.set_capacity(MAX_SWAPCHAIN_BUFFERS);
            for (u32 i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
                auto commandBuffer = Graphics::RZDrawCommandBuffer::Create();
                commandBuffer->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Frame Draw Command Buffer: #" + std::to_string(i)));
                m_DrawCommandBuffers.push_back(commandBuffer);
            }

            // Cache the reference to the Vulkan context to avoid frequent calling
            m_Context = VKContext::Get();
        }

        void VKRenderContext::AcquireImageAPIImpl(RZSemaphore* signalSemaphore)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the next image to present
            m_Context->getSwapchain()->acquireNextImage(VK_NULL_HANDLE);
            //m_CurrentCommandBuffer = m_DrawCommandBuffers[RHI::Get().GetSwapchain()->getCurrentBackBufferImageIndex()];
            m_CurrentCommandBuffer = m_DrawCommandBuffers.front();
            m_DrawCommandBuffers.pop_front();
            m_DrawCommandBuffers.push_back(m_CurrentCommandBuffer);

// Update VMA for Budget Queries
#if RAZIX_USE_VMA
    #ifndef RAZIX_DISTRIBUTION    // Only for debugging purposes
            vmaSetCurrentFrameIndex(VKDevice::Get().getVMA(), RHI::Get().GetSwapchain()->getCurrentFrameIndex());

                // Get Heap Statistics

    #endif
#endif
        }

        void VKRenderContext::BeginAPIImpl(RZDrawCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Begin recording to the command buffer
            m_CurrentCommandBuffer = cmdBuffer;

            m_CurrentCommandBuffer->BeginRecording();
        }

        void VKRenderContext::SubmitImpl(RZDrawCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // End the command buffer recording
            cmdBuffer->EndRecording();
            // Stack up the recorded command buffers for execution
            m_CommandQueue.push_back(cmdBuffer);
        }

        void VKRenderContext::SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

#if 0
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
#endif
        }

        void VKRenderContext::PresentAPIImpl(RZSemaphore* waitSemaphore)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

#if 0
            auto frameIdx     = RHI::Get().GetSwapchain()->getCurrentImageIndex();
            auto prevFrameIdx = frameIdx > 0 ? frameIdx - 1 : 2;
            m_Context->getSwapchain()->present(waitSemaphore ? *(VkSemaphore*) waitSemaphore->getHandle(frameIdx) : VK_NULL_HANDLE);
#endif

            m_Context->getSwapchain()->submitGraphicsAndFlip(m_CommandQueue);
            m_CommandQueue.clear();
        }

        void VKRenderContext::BindPipelineImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer)
        {
            auto pp = RZResourceManager::Get().getPool<RZPipeline>().get(pipeline);
            pp->Bind(cmdBuffer);
        }

        void VKRenderContext::BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto pp = RZResourceManager::Get().getPool<RZPipeline>().get(pipeline);
            //static_cast<VKPipeline*>(pp)->getPipelineLayout();

            const auto vkDescSet = static_cast<const VKDescriptorSet*>(descriptorSet)->getDescriptorSet();
            vkCmdBindDescriptorSets(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VKPipeline*>(pp)->getPipelineLayout(), setIdx, 1, &vkDescSet, 0, nullptr);
        }

        void VKRenderContext::BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets, u32 startSetIdx)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            u32 numDesciptorSets = 0;

            auto pp = RZResourceManager::Get().getPool<RZPipeline>().get(pipeline);

            for (auto descriptorSet: descriptorSets) {
                if (descriptorSet) {
                    auto vkDescSet                        = static_cast<VKDescriptorSet*>(descriptorSet);
                    m_DescriptorSetPool[numDesciptorSets] = vkDescSet->getDescriptorSet();
                    numDesciptorSets++;
                }
            }
            vkCmdBindDescriptorSets(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VKPipeline*>(pp)->getPipelineLayout(), startSetIdx, numDesciptorSets, m_DescriptorSetPool, 0, nullptr);
        }

        void VKRenderContext::BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets, u32 startSetIdx)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            u32 numDesciptorSets = 0;

            auto pp = RZResourceManager::Get().getPool<RZPipeline>().get(pipeline);

            for (u32 i = 0; i < totalSets; i++) {
                auto set = descriptorSets[i];
                if (set) {
                    const auto vkDescSet                  = static_cast<const VKDescriptorSet*>(set);
                    m_DescriptorSetPool[numDesciptorSets] = vkDescSet->getDescriptorSet();
                    numDesciptorSets++;
                }
            }
            vkCmdBindDescriptorSets(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VKPipeline*>(pp)->getPipelineLayout(), startSetIdx, numDesciptorSets, m_DescriptorSetPool, 0, nullptr);
        }

        void VKRenderContext::SetScissorRectImpl(RZDrawCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(width != 0 || height != 0, "Scissor Rect Width or Height cannot be zero!")

            VkRect2D scissorRect{};
            scissorRect.offset.x      = x;
            scissorRect.offset.y      = y;
            scissorRect.extent.width  = width;
            scissorRect.extent.height = height;

            vkCmdSetScissor(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), 0, 1, &scissorRect);
        }

        void VKRenderContext::EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto pp = RZResourceManager::Get().getPool<RZPipeline>().get(pipeline);

            // Bind the Bindless Descriptor Set
            if (VKDevice::Get().isBindlessSupported()) {
                const auto set = VKDevice::Get().getBindlessDescriptorSet();
                vkCmdBindDescriptorSets(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VKPipeline*>(pp)->getPipelineLayout(), BindingTable_System::SET_IDX_BINDLESS_RESOURCES_START, 1, &set, 0, nullptr);
            }
        }

        void VKRenderContext::BeginRenderingImpl(RZDrawCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VkRenderingInfoKHR renderingInfoKHR{};
            renderingInfoKHR.sType             = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
            renderingInfoKHR.renderArea.offset = {0, 0};

            if (renderingInfo.resolution == Resolution::kCustom)
                renderingInfoKHR.renderArea.extent = {renderingInfo.extent.x, renderingInfo.extent.y};
            else if (renderingInfo.resolution == Resolution::kWindow)
                renderingInfoKHR.renderArea.extent = {m_Width, m_Height};
            else {
                auto& res                          = ResolutionToExtentsMap[renderingInfo.resolution];
                renderingInfoKHR.renderArea.extent = {res.x, res.y};
            }

            RAZIX_ASSERT((renderingInfoKHR.renderArea.extent.width != 0 && renderingInfoKHR.renderArea.extent.height != 0), "[Vulkan] Rendering width or height cannot be zero!")

            // Update the command buffer viewport here
            SetViewport(cmdBuffer, 0, 0, renderingInfoKHR.renderArea.extent.width, renderingInfoKHR.renderArea.extent.height);
            SetScissorRect(cmdBuffer, 0, 0, renderingInfoKHR.renderArea.extent.width, renderingInfoKHR.renderArea.extent.height);

            renderingInfoKHR.layerCount = renderingInfo.layerCount;

            std::vector<VkRenderingAttachmentInfo> colorAttachments;

            for (auto& attachment: renderingInfo.colorAttachments) {
                RZTexture* colorAttachment = RZResourceManager::Get().getPool<RZTexture>().get(attachment.first);

                // Resize attachments when resized
                if (renderingInfo.resize) {
                    if (m_Width != colorAttachment->getWidth() || m_Height != colorAttachment->getHeight())
                        colorAttachment->Resize(renderingInfoKHR.renderArea.extent.width, renderingInfoKHR.renderArea.extent.height);
                }

                // Fill the color attachments first
                VkRenderingAttachmentInfoKHR attachInfo{};
                attachInfo.sType     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
                auto apiHandle       = static_cast<VkDescriptorImageInfo*>(colorAttachment->GetAPIHandlePtr());
                attachInfo.imageView = apiHandle->imageView;

                // Don't do this here, done manually bu the FG and user land code

                if (colorAttachment->getFormat() == TextureFormat::SCREEN) {
                    auto vkImage = static_cast<VKTexture*>(colorAttachment);
                    if (vkImage->getLayout() != VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL) {
                        VKUtilities::TransitionImageLayout(vkImage->getImage(), VKUtilities::TextureFormatToVK(vkImage->getFormat()), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL);
                        vkImage->setImageLayout(VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL);
                    }
                    attachInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                } else
                    attachInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;

                if (attachment.second.clear) {
                    attachInfo.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    attachInfo.storeOp = VK_ATTACHMENT_STORE_OP_NONE;
                } else {
                    attachInfo.loadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;    // Well don't discard stuff we render on top of what was rendered previously
                    attachInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                }

                auto clearColor = Graphics::ClearColorFromPreset(attachment.second.clearColor);

                memcpy(attachInfo.clearValue.color.float32, &clearColor, sizeof(glm::vec4));
                colorAttachments.push_back(attachInfo);
            }

            renderingInfoKHR.colorAttachmentCount = static_cast<u32>(colorAttachments.size());
            renderingInfoKHR.pColorAttachments    = colorAttachments.data();

            // Depth Attachment
            VkRenderingAttachmentInfoKHR attachInfo{};
            if (renderingInfo.depthAttachment.first.isValid()) {
                RZTexture* depthAttachment = RZResourceManager::Get().getPool<RZTexture>().get(renderingInfo.depthAttachment.first);
                // Depth attachment resize
                if (renderingInfo.resize) {
                    if (m_Width != depthAttachment->getWidth() || m_Height != depthAttachment->getHeight())
                        depthAttachment->Resize(renderingInfoKHR.renderArea.extent.width, renderingInfoKHR.renderArea.extent.height);
                }

                // Fill the color attachments first
                attachInfo.sType                 = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
                VkDescriptorImageInfo* apiHandle = (VkDescriptorImageInfo*) (depthAttachment->GetAPIHandlePtr());
                attachInfo.imageView             = apiHandle->imageView;

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

            CmdBeginRenderingKHR(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), &renderingInfoKHR);
        }

        void VKRenderContext::EndRenderingImpl(RZDrawCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            CmdEndRenderingKHR(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer());
        }

        void VKRenderContext::BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            std::vector<VkWriteDescriptorSet> writeDescriptorSets;

            auto pp = RZResourceManager::Get().getPool<RZPipeline>().get(pipeline);

            int descriptorWritesCount = 0;
            int imageIndex            = 0;
            int index                 = 0;

            // Source : https://github.com/SaschaWillems/Vulkan/blob/master/examples/pushdescriptors/pushdescriptors.cpp
            // Instead of preparing the descriptor sets up-front, using push descriptors we can set (push) them inside of a command buffer
            // This allows a more dynamic approach without the need to create descriptor sets for each model
            // Note: dstSet for each descriptor set write is left at zero as this is ignored when using push descriptors

            for (auto& desc: descriptors) {
                VkWriteDescriptorSet writeSet{};

                for (auto& descriptor: descriptors) {
                    if (descriptor.bindingInfo.type == DescriptorType::ImageSamplerCombined) {
                        const RZTexture* texturePtr = RZResourceManager::Get().getPool<RZTexture>().get(descriptor.texture);

                        VkDescriptorImageInfo& des = *static_cast<VkDescriptorImageInfo*>(texturePtr->GetAPIHandlePtr());

                        VkDescriptorImageInfo imageInfo{};
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        imageInfo.imageView   = des.imageView;
                        imageInfo.sampler     = des.sampler;

                        writeSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writeSet.dstSet          = 0;
                        writeSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        writeSet.dstBinding      = descriptor.bindingInfo.location.binding;
                        writeSet.pImageInfo      = &imageInfo;
                        writeSet.descriptorCount = 1;

                        imageIndex++;
                        descriptorWritesCount++;
                    } else {
                        VkDescriptorBufferInfo bufferInfo{};

                        auto uboresource  = RZResourceManager::Get().getUniformBufferResource(descriptor.uniformBuffer);
                        auto buffer       = static_cast<VKUniformBuffer*>(uboresource);
                        bufferInfo.buffer = buffer->getBuffer();
                        bufferInfo.offset = descriptor.offset;
                        bufferInfo.range  = buffer->getSize();

                        writeSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writeSet.dstSet          = 0;
                        writeSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        writeSet.dstBinding      = descriptor.bindingInfo.location.binding;
                        writeSet.pBufferInfo     = &bufferInfo;
                        writeSet.descriptorCount = 1;

                        index++;
                        descriptorWritesCount++;
                    }
                }

                writeDescriptorSets.push_back(writeSet);
            }
            CmdPushDescriptorSetKHR(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VKPipeline*>(pp)->getPipelineLayout(), 0, static_cast<u32>(writeDescriptorSets.size()), writeDescriptorSets.data());
        }

        void VKRenderContext::DrawAPIImpl(RZDrawCommandBuffer* cmdBuffer, u32 count, DataType /*= DataType::UNSIGNED_INT*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZEngine::Get().GetStatistics().NumDrawCalls++;
            RZEngine::Get().GetStatistics().Draws++;
            vkCmdDraw(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), count, 1, 0, 0);
        }

        void VKRenderContext::DrawIndexedAPIImpl(RZDrawCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, int32_t vertexOffset, u32 firstInstance)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZEngine::Get().GetStatistics().NumDrawCalls++;
            RZEngine::Get().GetStatistics().IndexedDraws++;
            vkCmdDrawIndexed(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        }

        void VKRenderContext::DestroyAPIImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
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
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return static_cast<RZSwapchain*>(VKContext::Get()->getSwapchain().get());
        }

        void VKRenderContext::BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, RZPushConstant pushConstant)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto pp = RZResourceManager::Get().getPool<RZPipeline>().get(pipeline);

            //for (auto& pushConstant: pushConstants) {
            vkCmdPushConstants(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), static_cast<VKPipeline*>(pp)->getPipelineLayout(), VKUtilities::ShaderStageToVK(pushConstant.shaderStage), pushConstant.offset, pushConstant.size, pushConstant.data);
            //}
        }

        void VKRenderContext::SetDepthBiasImpl(RZDrawCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            f32 depthBiasConstant = 1.25f;
            // Slope depth bias factor, applied depending on polygon's slope
            f32 depthBiasSlope = 1.75f;
            vkCmdSetDepthBias(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), depthBiasConstant, 0.0f, depthBiasSlope);
        }

        void VKRenderContext::InsertImageMemoryBarrierImpl(RZDrawCommandBuffer* cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the actual resource
            RZTexture* textureResource = RZResourceManager::Get().getPool<RZTexture>().get(texture);
            VKTexture* vkTexture       = static_cast<VKTexture*>(textureResource);

            // Update with the new layout
            vkTexture->setImageLayout((VkImageLayout) VKUtilities::EngineImageLayoutToVK(imgBarrierInfo.dstLayout));

            VkImageMemoryBarrier barrier = {};
            barrier.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout            = (VkImageLayout) VKUtilities::EngineImageLayoutToVK(imgBarrierInfo.srcLayout);
            barrier.newLayout            = (VkImageLayout) VKUtilities::EngineImageLayoutToVK(imgBarrierInfo.dstLayout);
            barrier.srcQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
            barrier.image                = vkTexture->getImage();
            barrier.srcAccessMask        = VKUtilities::EngineMemoryAcsessMaskToVK(imgBarrierInfo.srcAccess);
            barrier.dstAccessMask        = VKUtilities::EngineMemoryAcsessMaskToVK(imgBarrierInfo.dstAccess);

            auto format = VKUtilities::TextureFormatToVK(vkTexture->getFormat());

            if (format >= 124 && format <= 130)    // All possible depth formats
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            else
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel   = textureResource->getCurrentMipLevel();
            barrier.subresourceRange.levelCount     = textureResource->getMipsCount();
            barrier.subresourceRange.baseArrayLayer = textureResource->getCurrentArrayLayer();
            barrier.subresourceRange.layerCount     = textureResource->getLayersCount();

            vkCmdPipelineBarrier(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), VKUtilities::EnginePipelineStageToVK(pipelineBarrierInfo.startExecutionStage), VKUtilities::EnginePipelineStageToVK(pipelineBarrierInfo.endExecutionStage), 0, 0, nullptr, 0, nullptr, 1, &barrier);
        }

        void VKRenderContext::InsertBufferMemoryBarrierImpl(RZDrawCommandBuffer* cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZUniformBuffer* bufferResource = RZResourceManager::Get().getPool<RZUniformBuffer>().get(buffer);
            VKUniformBuffer* vkBuffer       = static_cast<VKUniformBuffer*>(bufferResource);

            VkBufferMemoryBarrier barrier = {};
            barrier.sType                 = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            barrier.srcAccessMask         = VKUtilities::EngineMemoryAcsessMaskToVK(bufBarrierInfo.srcAccess);
            barrier.dstAccessMask         = VKUtilities::EngineMemoryAcsessMaskToVK(bufBarrierInfo.dstAccess);
            barrier.srcQueueFamilyIndex   = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex   = VK_QUEUE_FAMILY_IGNORED;
            barrier.buffer                = vkBuffer->getBuffer();
            barrier.offset                = 0;
            barrier.size                  = vkBuffer->getSize();

            vkCmdPipelineBarrier(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), VKUtilities::EnginePipelineStageToVK(pipelineBarrierInfo.startExecutionStage), VKUtilities::EnginePipelineStageToVK(pipelineBarrierInfo.endExecutionStage), 0, 0, nullptr, 1, &barrier, 0, nullptr);
        }

        void VKRenderContext::CopyTextureResourceImpl(RZDrawCommandBuffer* cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTexture)
        {
            auto srcTextureResource = RZResourceManager::Get().getTextureResource(srcTexture);
            auto vkSrcTexture       = static_cast<VKTexture*>(srcTextureResource);

            auto dstTextureResource = RZResourceManager::Get().getTextureResource(dstTexture);
            auto vkDstTexture       = static_cast<VKTexture*>(dstTextureResource);

            // TODO: Supports all kinds of texture type copies
            VkImageCopy imageCopyRegion               = {};
            imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCopyRegion.srcSubresource.layerCount = 1;
            imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCopyRegion.dstSubresource.layerCount = 1;
            imageCopyRegion.extent.width              = vkSrcTexture->getWidth();
            imageCopyRegion.extent.height             = vkSrcTexture->getHeight();
            imageCopyRegion.extent.depth              = 1;

            vkCmdCopyImage(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), vkSrcTexture->getImage(), /*vkSrcTexture->getLayout()*/ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, vkDstTexture->getImage(), /*vkDstTexture->getLayout()*/ VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);
        }

        void VKRenderContext::SetViewportImpl(RZDrawCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            RAZIX_ASSERT(width != 0 || height != 0, "Viewport Width or Height cannot be zero!")

            VkViewport viewport = {};
            viewport.x          = static_cast<f32>(x);
            viewport.y          = static_cast<f32>(y);
            viewport.width      = static_cast<f32>(width);
            viewport.height     = static_cast<f32>(height);
            viewport.minDepth   = 0.0f;
            viewport.maxDepth   = 1.0f;

            vkCmdSetViewport(static_cast<VKDrawCommandBuffer*>(cmdBuffer)->getBuffer(), 0, 1, &viewport);
        }

        void VKRenderContext::SetCmdCheckpointImpl(RZDrawCommandBuffer* cmdbuffer, void* markerData)
        {
            RAZIX_CORE_WARN("Marker Data set : {0} at memory location : {1}", *static_cast<std::string*>(markerData), markerData);

            auto func = (PFN_vkCmdSetCheckpointNV) vkGetDeviceProcAddr(VKDevice::Get().getDevice(), "vkCmdSetCheckpointNV");
            if (func != nullptr)
                func(static_cast<VKDrawCommandBuffer*>(cmdbuffer)->getBuffer(), markerData);
        }
    }    // namespace Graphics
}    // namespace Razix
