// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKDescriptorSet.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKRenderContext.h"
#include "Razix/Platform/API/Vulkan/VKSampler.h"
#include "Razix/Platform/API/Vulkan/VKSwapchain.h"
#include "Razix/Platform/API/Vulkan/VKTexture.h"
#include "Razix/Platform/API/Vulkan/VKUniformBuffer.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix {
    namespace Gfx {

        VKDescriptorSet::VKDescriptorSet(const RZDescriptorSetDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
            : m_DescriptorPool(VK_NULL_HANDLE)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Desc = desc;
            // This also works for set index since all the descriptors will have the same set idx
            //m_Desc.setIdx = desc.descriptors[0].bindingInfo.location.set;

            // Descriptor sets can't be created directly, they must be allocated from a pool like command buffers i.e. use a descriptor pool to allocate the descriptor sets
            // We first need to describe which descriptor types our descriptor sets are going to contain and how many of them, we allocate a pool for each type of descriptor
            // Also we use one set for each frame, because it binds the with command buffer state, we will need as many uniform buffers and textures and the same amount of
            // descriptor sets to properly update the resources it points to

            std::vector<VkDescriptorSetLayoutBinding> setLayoutBindingInfos;

            for (auto descriptor: m_Desc.descriptors) {
                VkDescriptorSetLayoutBinding setLayoutBindingInfo = {};
                setLayoutBindingInfo.binding                      = descriptor.bindingInfo.location.binding;
                setLayoutBindingInfo.descriptorCount              = 1;    // descriptorCount is the number of descriptors contained in the binding, accessed in a shader as an array, if any (useful for Animation aka JointTransforms)
                setLayoutBindingInfo.descriptorType               = VKUtilities::DescriptorTypeToVK(descriptor.bindingInfo.type);
                setLayoutBindingInfo.stageFlags                   = VKUtilities::ShaderStageToVK(descriptor.bindingInfo.stage);

                setLayoutBindingInfos.push_back(setLayoutBindingInfo);
            }

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<u32>(setLayoutBindingInfos.size());
            layoutInfo.pBindings    = setLayoutBindingInfos.data();

            // Descriptor set layouts can be compatible if they are the same even if they are created in two different places.
            // Reference : (https://vkguide.dev/docs/chapter-4/descriptors/#descriptor-set-layout)
            // So even though they are already created in VKShader doing it again will not cause any binding issues, also cause
            // of this Shader and Descriptors API can stay decoupled which is a super good thing in terms of design which can spiral
            // into cyclic dependency real quick especially shaders and sets
            if (VK_CHECK_RESULT(vkCreateDescriptorSetLayout(VKDevice::Get().getDevice(), &layoutInfo, nullptr, &m_SetLayout)))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create descriptor set layout!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Successfully created descriptor set layout");

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (uint64_t) m_SetLayout)

            VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
            descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptorSetAllocateInfo.descriptorPool     = VKDevice::Get().getGlobalDescriptorPool();
            descriptorSetAllocateInfo.descriptorSetCount = 1;
            descriptorSetAllocateInfo.pSetLayouts        = &m_SetLayout;

            if (VK_CHECK_RESULT(vkAllocateDescriptorSets(VKDevice::Get().getDevice(), &descriptorSetAllocateInfo, &m_DescriptorSet)))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create descriptor sets!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Descriptor sets successfully created!");

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_DESCRIPTOR_SET, (uint64_t) m_DescriptorSet);

            // TODO: No point in having these pools for DescriptorXXXInfo we can have a temporary one in the loop
            m_BufferInfoPool         = new VkDescriptorBufferInfo[MAX_BUFFER_INFOS];
            m_ImageInfoPool          = new VkDescriptorImageInfo[MAX_COMBINED_IMAGE_INFOS];
            m_WriteDescriptorSetPool = new VkWriteDescriptorSet[MAX_WRITE_DESCRIPTORS];

            VKUtilities::VKCreateSamplerDesc defaultSamplerDesc = {};
            m_DefaultSampler                                    = VKUtilities::CreateImageSampler(defaultSamplerDesc RZ_DEBUG_NAME_TAG_STR_E_ARG("Default Combined Sampler"));

            // Initial update
            UpdateSet(m_Desc.descriptors);
        }

        void VKDescriptorSet::UpdateSet(const std::vector<RZDescriptor>& descriptors)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            int descriptorWritesCount = 0;
            int imageWriteIdx         = 0;
            int uniformBufferWriteIdx = 0;

            for (auto& descriptor: descriptors) {
                switch (descriptor.bindingInfo.type) {
                    case DescriptorType::kUniformBuffer: {
                        auto uboresource = RZResourceManager::Get().getUniformBufferResource(descriptor.uniformBuffer);
                        auto buffer      = static_cast<VKUniformBuffer*>(uboresource);
                        if (buffer) {
                            m_BufferInfoPool[uniformBufferWriteIdx].buffer = buffer->getBuffer();
                            m_BufferInfoPool[uniformBufferWriteIdx].offset = descriptor.offset;
                            m_BufferInfoPool[uniformBufferWriteIdx].range  = buffer->getSize();
                        } else {
                            m_BufferInfoPool[uniformBufferWriteIdx].buffer = VKDevice::Get().GetDummyBuffer();
                            m_BufferInfoPool[uniformBufferWriteIdx].offset = 0;
                            m_BufferInfoPool[uniformBufferWriteIdx].range  = VK_WHOLE_SIZE;
                        }

                        VkWriteDescriptorSet writeDescriptorSet{};
                        writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writeDescriptorSet.dstSet          = m_DescriptorSet;
                        writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        writeDescriptorSet.dstBinding      = descriptor.bindingInfo.location.binding;
                        writeDescriptorSet.pBufferInfo     = &m_BufferInfoPool[uniformBufferWriteIdx];
                        writeDescriptorSet.descriptorCount = 1;

                        m_WriteDescriptorSetPool[descriptorWritesCount] = writeDescriptorSet;
                        uniformBufferWriteIdx++;
                        descriptorWritesCount++;
                    } break;
                    case DescriptorType::kImageSamplerCombined: {
                        const RZTexture* texturePtr = RZResourceManager::Get().getPool<RZTexture>().get(descriptor.texture);

                        if (texturePtr) {
                            const VKTexture* backendPtr = static_cast<const VKTexture*>(texturePtr);

                            m_ImageInfoPool[imageWriteIdx].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                            m_ImageInfoPool[imageWriteIdx].imageView   = backendPtr->getFullSRVImageView();
                            RAZIX_CORE_WARN("[Vulkan] found CombinedImageSampler resource, using a default sampler, refrain from using this type");
                            m_ImageInfoPool[imageWriteIdx].sampler = m_DefaultSampler;
                        } else {
                            RAZIX_CORE_WARN("[Vulkan] No sampler resource provided, using a default sampler");
                            m_ImageInfoPool[imageWriteIdx].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                            m_ImageInfoPool[imageWriteIdx].imageView   = VKDevice::Get().GetDummyImageView();
                            m_ImageInfoPool[imageWriteIdx].sampler     = m_DefaultSampler;
                        }

                        VkWriteDescriptorSet writeDescriptorSet = {};
                        writeDescriptorSet.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writeDescriptorSet.dstSet               = m_DescriptorSet;
                        writeDescriptorSet.descriptorType       = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        writeDescriptorSet.dstBinding           = descriptor.bindingInfo.location.binding;
                        writeDescriptorSet.pImageInfo           = &m_ImageInfoPool[imageWriteIdx];
                        writeDescriptorSet.descriptorCount      = 1;

                        m_WriteDescriptorSetPool[descriptorWritesCount] = writeDescriptorSet;
                        imageWriteIdx++;
                        descriptorWritesCount++;
                    } break;
                    case DescriptorType::kTexture: {
                        RZTexture* texturePtr = RZResourceManager::Get().getPool<RZTexture>().get(descriptor.texture);

                        if (texturePtr) {
                            VKTexture* backendPtr = static_cast<VKTexture*>(texturePtr);
                            // only if it's not a UAV at all
                            ResourceViewHint viewHints = (ResourceViewHint) backendPtr->getDescription().initResourceViewHints;
                            if ((viewHints & kUAV) != kUAV)
                                backendPtr->transitonImageLayoutToSRV();

                            m_ImageInfoPool[imageWriteIdx].imageLayout = backendPtr->getImageLayoutValue();
                            m_ImageInfoPool[imageWriteIdx].imageView   = backendPtr->getFullSRVImageView();
                        } else {
                            m_ImageInfoPool[imageWriteIdx].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                            m_ImageInfoPool[imageWriteIdx].imageView   = VKDevice::Get().GetDummyImageView();
                        }

                        VkWriteDescriptorSet writeDescriptorSet = {};
                        writeDescriptorSet.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writeDescriptorSet.dstSet               = m_DescriptorSet;
                        writeDescriptorSet.descriptorType       = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;    // Texture
                        writeDescriptorSet.dstBinding           = descriptor.bindingInfo.location.binding;
                        writeDescriptorSet.pImageInfo           = &m_ImageInfoPool[imageWriteIdx];
                        writeDescriptorSet.descriptorCount      = 1;

                        m_WriteDescriptorSetPool[descriptorWritesCount] = writeDescriptorSet;
                        imageWriteIdx++;
                        descriptorWritesCount++;
                    } break;
                    case DescriptorType::kRWTexture: {
                        RZTexture* texturePtr = RZResourceManager::Get().getPool<RZTexture>().get(descriptor.texture);

                        if (texturePtr) {
                            VKTexture* backendPtr = static_cast<VKTexture*>(texturePtr);
                            backendPtr->transitonImageLayoutToUAV();
                            u32 currentMipLevel = backendPtr->getCurrentMipLevel();
                            u32 baseArrayLevel  = backendPtr->getBaseArrayLayer();

                            VkImageView imgView = VK_NULL_HANDLE;

                            if (currentMipLevel > 0 && baseArrayLevel > 0)
                                imgView = backendPtr->getLayerMipUAVImageView(baseArrayLevel, currentMipLevel);
                            else if (currentMipLevel > 0 && baseArrayLevel == 0)
                                imgView = backendPtr->getMipUAVImageView(currentMipLevel);
                            else
                                imgView = backendPtr->getFullUAVImageView();

                            m_ImageInfoPool[imageWriteIdx].imageLayout = backendPtr->getImageLayoutValue();
                            m_ImageInfoPool[imageWriteIdx].imageView   = imgView;
                        } else {
                            m_ImageInfoPool[imageWriteIdx].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                            m_ImageInfoPool[imageWriteIdx].imageView   = VKDevice::Get().GetDummyImageView();
                        }

                        VkWriteDescriptorSet writeDescriptorSet = {};
                        writeDescriptorSet.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writeDescriptorSet.dstSet               = m_DescriptorSet;
                        writeDescriptorSet.descriptorType       = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;    // RW Texture
                        writeDescriptorSet.dstBinding           = descriptor.bindingInfo.location.binding;
                        writeDescriptorSet.pImageInfo           = &m_ImageInfoPool[imageWriteIdx];
                        writeDescriptorSet.descriptorCount      = 1;

                        m_WriteDescriptorSetPool[descriptorWritesCount] = writeDescriptorSet;
                        imageWriteIdx++;
                        descriptorWritesCount++;
                    } break;
                    case DescriptorType::kSampler: {
                        const RZSampler* samplerPtr = RZResourceManager::Get().getPool<RZSampler>().get(descriptor.sampler);

                        if (samplerPtr) {
                            const VKSampler* backendPtr            = static_cast<const VKSampler*>(samplerPtr);
                            m_ImageInfoPool[imageWriteIdx].sampler = backendPtr->getSampler();
                        } else {
                            RAZIX_CORE_WARN("[Vulkan] No sampler resource provided, using a default sampler");
                            m_ImageInfoPool[imageWriteIdx].sampler = m_DefaultSampler;
                        }

                        VkWriteDescriptorSet writeDescriptorSet = {};
                        writeDescriptorSet.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writeDescriptorSet.dstSet               = m_DescriptorSet;
                        writeDescriptorSet.descriptorType       = VK_DESCRIPTOR_TYPE_SAMPLER;
                        writeDescriptorSet.dstBinding           = descriptor.bindingInfo.location.binding;
                        writeDescriptorSet.pImageInfo           = &m_ImageInfoPool[imageWriteIdx];
                        writeDescriptorSet.descriptorCount      = 1;

                        m_WriteDescriptorSetPool[descriptorWritesCount] = writeDescriptorSet;
                        imageWriteIdx++;
                        descriptorWritesCount++;
                    } break;
                    case DescriptorType::kRWTyped:
                    case DescriptorType::kStructured:
                    case DescriptorType::kRWStructured:
                    case DescriptorType::kByteAddress:
                    case DescriptorType::kRWByteAddress:
                    case DescriptorType::kAppendStructured:
                    case DescriptorType::kConsumeStructured:
                    case DescriptorType::kRWStructuredCounter:
                    case DescriptorType::kRTAccelerationStructure:
                    default:
                        RAZIX_CORE_ERROR("[VULKAN] Unknow descriptor type!");
                        break;
                }
            }
            vkUpdateDescriptorSets(VKDevice::Get().getDevice(), descriptorWritesCount, m_WriteDescriptorSetPool, 0, nullptr);
        }

        //-------------------------------------------------------------------------------------------
        RAZIX_CLEANUP_RESOURCE_IMPL(VKDescriptorSet)
        {
            if (m_DefaultSampler != VK_NULL_HANDLE)
                vkDestroySampler(VKDevice::Get().getDevice(), m_DefaultSampler, nullptr);

            vkDestroyDescriptorSetLayout(VKDevice::Get().getDevice(), m_SetLayout, nullptr);
        }
    }    // namespace Gfx
}    // namespace Razix
