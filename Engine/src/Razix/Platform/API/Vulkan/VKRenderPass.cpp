// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKRenderPass.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"

#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"
#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "Razix/Platform/API/Vulkan/VKFramebuffer.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix {
    namespace Graphics {

        VKRenderPass::VKRenderPass(const RenderPassInfo& renderPassInfo RZ_DEBUG_NAME_TAG_E_ARG)
            : m_RenderPass(VK_NULL_HANDLE), m_ClearValue(nullptr), m_DepthOnly(false), m_ClearDepth(false)
        {
            m_AttachmentsCount      = 0;
            m_ColorAttachmentsCount = 0;

            m_AttachmentTypes = renderPassInfo.attachmentInfos;

            init(renderPassInfo RZ_DEBUG_E_ARG_NAME);
        }

        VKRenderPass::~VKRenderPass()
        {
        }

        void VKRenderPass::BeginRenderPass(RZCommandBuffer* commandBuffer, glm::vec4 clearColor, RZFramebuffer* framebuffer, SubPassContents subpass, u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            commandBuffer->UpdateViewport(width, height);

            // TODO: choose the union type based on format type for vulkan SINT/UINT/Sf32 and update only that properly

            if (!m_DepthOnly) {
                for (sz i = 0; i < m_AttachmentsCount; i++) {
                    //if (m_AttachmentTypes[i].format == RZTextureProperties::Format::BGRA8_UNORM) {
                    m_ClearValue[i].color = {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
                    //m_ClearValue[i].color.int32[1] = m_AttachmentTypes[i].clearColor.y;
                    //m_ClearValue[i].color.int32[2] = m_AttachmentTypes[i].clearColor.x;
                    //m_ClearValue[i].color.int32[3] = m_AttachmentTypes[i].clearColor.w;
                    //} else if (m_AttachmentTypes[i].format == RZTextureProperties::Format::R32_INT) {
                    //m_ClearValue[i].color.int32[0] = -1;    //int32_t(m_AttachmentTypes[i].clearColor.x);
                    //m_ClearValue[i].color.int32[1] = -1;    //int32_t(m_AttachmentTypes[i].clearColor.y);
                    //m_ClearValue[i].color.int32[2] = -1;    //int32_t(m_AttachmentTypes[i].clearColor.x);
                    //m_ClearValue[i].color.int32[3] = -1;    //int32_t(m_AttachmentTypes[i].clearColor.w);
                    //}
                }
            }
            // Assuming that depth is always the last attachment, and we clear it with a constant value no customization can be done here this needs to be supported in future as needed
            m_ClearValue[m_AttachmentsCount - 1].depthStencil = VkClearDepthStencilValue{1.0f, 0};

            VkRenderPassBeginInfo rpBegin{};
            rpBegin.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rpBegin.pNext                    = NULL;
            rpBegin.renderPass               = m_RenderPass;
            rpBegin.framebuffer              = static_cast<VKFramebuffer*>(framebuffer)->getVkFramebuffer();
            rpBegin.renderArea.offset.x      = 0;
            rpBegin.renderArea.offset.y      = 0;
            rpBegin.renderArea.extent.width  = width;
            rpBegin.renderArea.extent.height = height;
            rpBegin.clearValueCount          = u32(m_AttachmentsCount);
            rpBegin.pClearValues             = m_ClearValue;

            vkCmdBeginRenderPass(static_cast<VKCommandBuffer*>(commandBuffer)->getBuffer(), &rpBegin, (subpass == INLINE ? VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS));
        }

        void VKRenderPass::EndRenderPass(RZCommandBuffer* commandBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            vkCmdEndRenderPass(static_cast<VKCommandBuffer*>(commandBuffer)->getBuffer());
        }

        void VKRenderPass::AddAttachment()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_UNIMPLEMENTED_METHOD
        }

        void VKRenderPass::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            delete[] m_ClearValue;
            vkDestroyRenderPass(VKDevice::Get().getDevice(), m_RenderPass, nullptr);
        }

        bool VKRenderPass::init(const RenderPassInfo& renderpassInfo RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // List of attachments that will be attached to the render pass
            std::vector<VkAttachmentDescription> attachments;

            // List of color and depth attachment references refer to the actual attachments like image is the attachment description and image view is the attachment reference(this is a really bad and vague analogy)
            std::vector<VkAttachmentReference> colourAttachmentReferences;
            std::vector<VkAttachmentReference> depthAttachmentReferences;

            // These will be enabled based on the type of the attachments available
            m_DepthOnly  = true;
            m_ClearDepth = false;

            for (u32 i = 0; i < renderpassInfo.attachmentCount; i++) {
                attachments.push_back(getAttachmentDescription(renderpassInfo.attachmentInfos[i], renderpassInfo.attachmentInfos[i].clear));

                if (renderpassInfo.attachmentInfos[i].type == RZTextureProperties::Type::Texture_2D) {
                    VkAttachmentReference colourAttachmentRef = {};
                    colourAttachmentRef.attachment            = u32(i);
                    colourAttachmentRef.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    colourAttachmentReferences.push_back(colourAttachmentRef);
                    m_DepthOnly = false;
                } else if (renderpassInfo.attachmentInfos[i].type == RZTextureProperties::Type::Texture_Depth) {
                    VkAttachmentReference depthAttachmentRef = {};
                    depthAttachmentRef.attachment            = u32(i);
                    depthAttachmentRef.layout                = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                    depthAttachmentReferences.push_back(depthAttachmentRef);
                    m_ClearDepth = renderpassInfo.attachmentInfos->clear;
                }
            }

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount    = static_cast<u32>(colourAttachmentReferences.size());
            subpass.pColorAttachments       = colourAttachmentReferences.data();
            subpass.pDepthStencilAttachment = depthAttachmentReferences.data();

            m_ColorAttachmentsCount = int(colourAttachmentReferences.size());

            std::vector<VkSubpassDependency> dependencies;

            if (!m_DepthOnly) {
                VkSubpassDependency dependency = {};
                dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
                dependency.dstSubpass          = 0;
                dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.srcAccessMask       = 0;
                dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependencies.push_back(dependency);
            } else {
                // We use 2 sub passes for a depth only render pass, especially for shadow mapping and early fragments tests
                std::array<VkSubpassDependency, 2> dependencies_array;

                dependencies_array[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
                dependencies_array[0].dstSubpass      = 0;
                dependencies_array[0].srcStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                dependencies_array[0].dstStageMask    = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                dependencies_array[0].srcAccessMask   = VK_ACCESS_SHADER_READ_BIT;
                dependencies_array[0].dstAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                dependencies_array[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

                dependencies_array[1].srcSubpass      = 0;
                dependencies_array[1].dstSubpass      = VK_SUBPASS_EXTERNAL;
                dependencies_array[1].srcStageMask    = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                dependencies_array[1].dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                dependencies_array[1].srcAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                dependencies_array[1].dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
                dependencies_array[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

                dependencies.push_back(dependencies_array[0]);
                dependencies.push_back(dependencies_array[1]);
            }

            VkRenderPassCreateInfo vkRenderpassCI{};
            vkRenderpassCI.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            vkRenderpassCI.attachmentCount = u32(renderpassInfo.attachmentCount);
            vkRenderpassCI.pAttachments    = attachments.data();
            vkRenderpassCI.subpassCount    = 1;
            vkRenderpassCI.pSubpasses      = &subpass;
            vkRenderpassCI.dependencyCount = static_cast<u32>(dependencies.size());
            vkRenderpassCI.pDependencies   = dependencies.data();

            if (VK_CHECK_RESULT(vkCreateRenderPass(VKDevice::Get().getDevice(), &vkRenderpassCI, nullptr, &m_RenderPass)))
                RAZIX_CORE_ERROR("[Vulkan] Cannot create ({0}) render pass ", renderpassInfo.name);
            else
                RAZIX_CORE_TRACE("[Vulkan] Successfully created render pass : {0}", renderpassInfo.name);

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_RENDER_PASS, (uint64_t) m_RenderPass);

            m_ClearValue       = new VkClearValue[renderpassInfo.attachmentCount];
            m_AttachmentsCount = renderpassInfo.attachmentCount;
            return true;
        }

        VkAttachmentDescription VKRenderPass::getAttachmentDescription(RenderPassAttachmentInfo info, bool clear /*= true*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VkAttachmentDescription attachment = {};
            if (info.type == RZTextureProperties::Type::Texture_2D) {
                attachment.format      = info.format == RZTextureProperties::Format::SCREEN ? VKContext::Get()->getSwapchain()->getColorFormat() : VKUtilities::TextureFormatToVK(info.format, true);
                attachment.finalLayout = info.format == RZTextureProperties::Format::SCREEN ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            } else if (info.type == RZTextureProperties::Type::Texture_Depth) {
                attachment.format      = VKUtilities::FindDepthFormat();
                attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            } else {
                RAZIX_CORE_WARN("[Vulkan] Unsupported texture type : {0}", static_cast<int>(info.type));
                return attachment;
            }

            if (clear) {
                attachment.loadOp        = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            } else {
                attachment.loadOp        = VK_ATTACHMENT_LOAD_OP_LOAD;    // Well don't discard stuff we render on top of what was presented previously
                attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                attachment.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }

            // We are always clearing the depth after every render pass so yeah beware of this make this optional for the render pass add explicit bool to control this
            if (info.type == RZTextureProperties::Type::Texture_Depth) {
                attachment.loadOp        = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            }

            attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
            attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.flags          = 0;

            return attachment;
        }
    }    // namespace Graphics
}    // namespace Razix