// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKPipeline.h"

#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKRenderPass.h"
#include "Razix/Platform/API/Vulkan/VKShader.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix {
    namespace Graphics {

        VKPipeline::VKPipeline(const PipelineInfo& pipelineInfo RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_Shader         = pipelineInfo.shader;
            m_PipelineLayout = static_cast<VKShader*>(m_Shader)->getPipelineLayout();

            init(pipelineInfo RZ_DEBUG_E_ARG_NAME);
        }

        void VKPipeline::Bind(RZCommandBuffer* commandBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            vkCmdBindPipeline(static_cast<VKCommandBuffer*>(commandBuffer)->getBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
        }

        void VKPipeline::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            vkDestroyPipeline(VKDevice::Get().getDevice(), m_Pipeline, nullptr);
        }

        void VKPipeline::init(const PipelineInfo& pipelineInfo RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            //----------------------------
            // Vertex Input Layout Stage
            //----------------------------
            VkVertexInputBindingDescription vertexBindingDescription{};
            vertexBindingDescription.binding   = 0;
            vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            vertexBindingDescription.stride    = m_Shader->getInputStride();

            // Get the input description information from the shader reflection
            const std::vector<VkVertexInputAttributeDescription>& vertexInputAttributeDescription = static_cast<VKShader*>(m_Shader)->getVertexAttribDescriptions();

            VkPipelineVertexInputStateCreateInfo vertexInputSCI{};
            vertexInputSCI.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputSCI.pNext                           = nullptr;
            vertexInputSCI.vertexBindingDescriptionCount   = 1;
            vertexInputSCI.pVertexBindingDescriptions      = &vertexBindingDescription;
            vertexInputSCI.vertexAttributeDescriptionCount = uint32_t(vertexInputAttributeDescription.size());
            vertexInputSCI.pVertexAttributeDescriptions    = vertexInputAttributeDescription.data();

            //----------------------------
            // Input Assembly Stage
            //----------------------------
            VkPipelineInputAssemblyStateCreateInfo inputAssemblySCI{};
            inputAssemblySCI.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssemblySCI.pNext                  = nullptr;
            inputAssemblySCI.primitiveRestartEnable = VK_FALSE;
            inputAssemblySCI.topology               = VKUtilities::DrawTypeToVK(pipelineInfo.drawType);

            //----------------------------
            // Viewport and Dynamic states
            //----------------------------

            std::vector<VkDynamicState> dynamicStateDescriptors;

            VkPipelineViewportStateCreateInfo viewportSCI{};
            viewportSCI.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportSCI.pNext         = nullptr;
            viewportSCI.viewportCount = 1;
            viewportSCI.scissorCount  = 1;
            viewportSCI.pScissors     = nullptr;
            viewportSCI.pViewports    = nullptr;
            dynamicStateDescriptors.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            dynamicStateDescriptors.push_back(VK_DYNAMIC_STATE_SCISSOR);

            if (pipelineInfo.depthBiasEnabled)
                dynamicStateDescriptors.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);

            VkPipelineDynamicStateCreateInfo dynamicStateCI{};
            dynamicStateCI.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicStateCI.pNext             = NULL;
            dynamicStateCI.dynamicStateCount = uint32_t(dynamicStateDescriptors.size());
            dynamicStateCI.pDynamicStates    = dynamicStateDescriptors.data();

            //----------------------------
            // Rasterizer Stage
            //----------------------------
            VkPipelineRasterizationStateCreateInfo rasterizationSCI{};
            rasterizationSCI.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizationSCI.pNext                   = nullptr;
            rasterizationSCI.cullMode                = VKUtilities::CullModeToVK(pipelineInfo.cullMode);
            rasterizationSCI.depthBiasClamp          = 0;
            rasterizationSCI.depthBiasConstantFactor = 0;
            rasterizationSCI.depthBiasEnable         = (pipelineInfo.depthBiasEnabled ? VK_TRUE : VK_FALSE);
            rasterizationSCI.depthBiasSlopeFactor    = 0;
            rasterizationSCI.depthClampEnable        = VK_FALSE;
            rasterizationSCI.frontFace               = VK_FRONT_FACE_CLOCKWISE;
            rasterizationSCI.lineWidth               = 1.0f;
            rasterizationSCI.polygonMode             = VKUtilities::PolygoneModeToVK(pipelineInfo.polygonMode);
            rasterizationSCI.rasterizerDiscardEnable = VK_FALSE;

            //----------------------------
            // Blend State Stage
            //----------------------------
            VkPipelineColorBlendStateCreateInfo colorBlendSCI{};
            colorBlendSCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlendSCI.pNext = NULL;
            colorBlendSCI.flags = 0;

            std::vector<VkPipelineColorBlendAttachmentState> blendAttachState;
            if (pipelineInfo.attachmentFormats.size() > 1)
                blendAttachState.resize(pipelineInfo.attachmentFormats.size() - 1);
            else
                blendAttachState.resize(pipelineInfo.attachmentFormats.size());

            for (unsigned int i = 0; i < blendAttachState.size(); i++) {
                blendAttachState[i]                = VkPipelineColorBlendAttachmentState();
                blendAttachState[i].colorWriteMask = 0x0f;
                blendAttachState[i].alphaBlendOp   = VK_BLEND_OP_ADD;
                blendAttachState[i].colorBlendOp   = VK_BLEND_OP_ADD;

                if (pipelineInfo.transparencyEnabled) {
                    blendAttachState[i].blendEnable         = VK_TRUE;
                    blendAttachState[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                    blendAttachState[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                    blendAttachState[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                    blendAttachState[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                } else {
                    blendAttachState[i].blendEnable         = VK_FALSE;
                    blendAttachState[i].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
                    blendAttachState[i].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
                    blendAttachState[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                    blendAttachState[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                }
            }

            colorBlendSCI.attachmentCount   = static_cast<uint32_t>(blendAttachState.size());
            colorBlendSCI.pAttachments      = blendAttachState.data();
            colorBlendSCI.logicOpEnable     = VK_FALSE;
            colorBlendSCI.logicOp           = VK_LOGIC_OP_NO_OP;
            colorBlendSCI.blendConstants[0] = 1.0f;
            colorBlendSCI.blendConstants[1] = 1.0f;
            colorBlendSCI.blendConstants[2] = 1.0f;
            colorBlendSCI.blendConstants[3] = 1.0f;

            //----------------------------
            // Depth Stencil Stage
            //----------------------------
            VkPipelineDepthStencilStateCreateInfo depthStencilSCI{};
            depthStencilSCI.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencilSCI.pNext                 = nullptr;
            depthStencilSCI.depthTestEnable       = VK_TRUE;
            depthStencilSCI.depthWriteEnable      = VK_TRUE;
            depthStencilSCI.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
            depthStencilSCI.depthBoundsTestEnable = VK_FALSE;
            depthStencilSCI.stencilTestEnable     = VK_FALSE;

            depthStencilSCI.back.failOp      = VK_STENCIL_OP_KEEP;
            depthStencilSCI.back.passOp      = VK_STENCIL_OP_KEEP;
            depthStencilSCI.back.compareOp   = VK_COMPARE_OP_ALWAYS;
            depthStencilSCI.back.compareMask = 0;
            depthStencilSCI.back.reference   = 0;
            depthStencilSCI.back.depthFailOp = VK_STENCIL_OP_KEEP;
            depthStencilSCI.back.writeMask   = 0;

            depthStencilSCI.minDepthBounds = 0;
            depthStencilSCI.maxDepthBounds = 0;
            depthStencilSCI.front          = depthStencilSCI.back;

            //----------------------------
            // Multi sample State
            //----------------------------
            VkPipelineMultisampleStateCreateInfo multiSampleSCI{};
            multiSampleSCI.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multiSampleSCI.pNext                 = nullptr;
            multiSampleSCI.pSampleMask           = nullptr;
            multiSampleSCI.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
            multiSampleSCI.sampleShadingEnable   = VK_FALSE;
            multiSampleSCI.alphaToCoverageEnable = VK_FALSE;
            multiSampleSCI.alphaToOneEnable      = VK_FALSE;
            multiSampleSCI.minSampleShading      = 0.0;

            //----------------------------
            // Dynamic Rendering KHR
            //----------------------------

            VkPipelineRenderingCreateInfoKHR renderingCI{};
            renderingCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
            std::vector<VkFormat> formats;
            uint32_t              c = 0;
            for (auto& attachment: pipelineInfo.attachmentFormats) {
                if (attachment == RZTexture::Format::DEPTH) {
                    renderingCI.depthAttachmentFormat = VKUtilities::FindDepthFormat();
                    continue;
                }
                formats.push_back(VKUtilities::TextureFormatToVK(attachment));
                c++;
            }
            renderingCI.colorAttachmentCount    = c;    //static_cast<uint32_t>(pipelineInfo.attachmentFormats.size());
            renderingCI.pColorAttachmentFormats = formats.data();

            //----------------------------
            // Graphics Pipeline
            //----------------------------
            VkGraphicsPipelineCreateInfo graphicsPipelineCI{};
            graphicsPipelineCI.sType                                  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            graphicsPipelineCI.pNext                                  = &renderingCI;
            graphicsPipelineCI.layout                                 = m_PipelineLayout;
            graphicsPipelineCI.basePipelineHandle                     = VK_NULL_HANDLE;
            graphicsPipelineCI.basePipelineIndex                      = -1;
            graphicsPipelineCI.pVertexInputState                      = &vertexInputSCI;
            graphicsPipelineCI.pInputAssemblyState                    = &inputAssemblySCI;
            graphicsPipelineCI.pRasterizationState                    = &rasterizationSCI;
            graphicsPipelineCI.pColorBlendState                       = &colorBlendSCI;
            graphicsPipelineCI.pTessellationState                     = nullptr;
            graphicsPipelineCI.pMultisampleState                      = &multiSampleSCI;
            graphicsPipelineCI.pDynamicState                          = &dynamicStateCI;
            graphicsPipelineCI.pViewportState                         = &viewportSCI;
            graphicsPipelineCI.pDepthStencilState                     = &depthStencilSCI;
            std::vector<VkPipelineShaderStageCreateInfo> shaderStages = static_cast<VKShader*>(m_Shader)->getShaderStages();
            graphicsPipelineCI.pStages                                = shaderStages.data();
            graphicsPipelineCI.stageCount                             = static_cast<uint32_t>(shaderStages.size());
            graphicsPipelineCI.renderPass                             = VK_NULL_HANDLE;    //static_cast<VKRenderPass*>(pipelineInfo.renderpass)->getVKRenderPass();

            // TODO: use pipeline cache
            if (VK_CHECK_RESULT(vkCreateGraphicsPipelines(VKDevice::Get().getDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCI, nullptr, &m_Pipeline)))
                RAZIX_CORE_ERROR("[Vulkan] Cannot create graphics pipeline!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Successfully created graphics pipeline!");

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_PIPELINE, (uint64_t) m_Pipeline);
        }
    }    // namespace Graphics
}    // namespace Razix