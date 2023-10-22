// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKDescriptorSet.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKRenderContext.h"
#include "Razix/Platform/API/Vulkan/VKSwapchain.h"
#include "Razix/Platform/API/Vulkan/VKTexture.h"
#include "Razix/Platform/API/Vulkan/VKUniformBuffer.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix {
    namespace Graphics {

        VKDescriptorSet::VKDescriptorSet(const std::vector<RZDescriptor>& descriptors, bool layoutTransition RZ_DEBUG_NAME_TAG_E_ARG)
            : m_DescriptorPool(VK_NULL_HANDLE)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // This also works for set index since all the descriptors will have the same set idx
            m_SetIdx = descriptors[0].bindingInfo.location.set;

            // Descriptor sets can't be created directly, they must be allocated from a pool like command buffers i.e. use a descriptor pool to allocate the descriptor sets
            // We first need to describe which descriptor types our descriptor sets are going to contain and how many of them, we allocate a pool for each type of descriptor
            // Also we use one set for each frame, because it binds the with command buffer state, we will need as many uniform buffers and textures and the same amount of
            // descriptor sets to properly update the resources it points to

            std::vector<VkDescriptorSetLayoutBinding> setLayoutBindingInfos;

            for (auto descriptor: descriptors) {
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
            if (VK_CHECK_RESULT(vkCreateDescriptorSetLayout(VKDevice::Get().getDevice(), &layoutInfo, nullptr, &setLayout)))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create descriptor set layout!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Successfully created descriptor set layout");

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (uint64_t) setLayout)

            VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
            descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptorSetAllocateInfo.descriptorPool     = VKDevice::Get().getGlobalDescriptorPool();
            descriptorSetAllocateInfo.descriptorSetCount = 1;
            descriptorSetAllocateInfo.pSetLayouts        = &setLayout;

            if (VK_CHECK_RESULT(vkAllocateDescriptorSets(VKDevice::Get().getDevice(), &descriptorSetAllocateInfo, &m_DescriptorSet)))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create descriptor sets!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Descriptor sets successfully created!");

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_DESCRIPTOR_SET, (uint64_t) m_DescriptorSet);

            // TODO: No point in having these pools for DescriptorXXXInfo we can have a temporary one in the loop
            m_BufferInfoPool         = new VkDescriptorBufferInfo[MAX_BUFFER_INFOS];
            m_ImageInfoPool          = new VkDescriptorImageInfo[MAX_IMAGE_INFOS];
            m_WriteDescriptorSetPool = new VkWriteDescriptorSet[MAX_WRITE_DESCTIPTORS];

            UpdateSet(descriptors, layoutTransition);
        }

        void VKDescriptorSet::UpdateSet(const std::vector<RZDescriptor>& descriptors, bool layoutTransition)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            int descriptorWritesCount = 0;
            {
                int imageIndex = 0;
                int index      = 0;

                for (auto& descriptor: descriptors) {
                    if (descriptor.bindingInfo.type == DescriptorType::ImageSamplerCombined) {
                        const RZTexture* texturePtr = RZResourceManager::Get().getPool<RZTexture>().get(descriptor.texture);

                        if (texturePtr) {
                            VkDescriptorImageInfo& des = *static_cast<VkDescriptorImageInfo*>(texturePtr->GetAPIHandlePtr());

#if 0
                        if (descriptor.texture->getType() == TextureType::Texture_2D) {
                            auto vkImage = static_cast<VKRenderTexture*>(descriptor.texture);
                            if (layoutTransition) {
                                VKUtilities::TransitionImageLayout(vkImage->getImage(), VKUtilities::TextureFormatToVK(vkImage->getFormat()), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                            }
                        }
#endif

                            m_ImageInfoPool[imageIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                            m_ImageInfoPool[imageIndex].imageView   = des.imageView;
                            m_ImageInfoPool[imageIndex].sampler     = des.sampler;
                        } else {
                            m_ImageInfoPool[imageIndex].imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                            m_ImageInfoPool[imageIndex].imageView   = VK_NULL_HANDLE;
                            m_ImageInfoPool[imageIndex].sampler     = VKTexture::CreateImageSampler();    // Use some default sampler!
                        }

                        VkWriteDescriptorSet writeDescriptorSet{};
                        writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writeDescriptorSet.dstSet          = m_DescriptorSet;
                        writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        writeDescriptorSet.dstBinding      = descriptor.bindingInfo.location.binding;
                        writeDescriptorSet.pImageInfo      = &m_ImageInfoPool[imageIndex];
                        writeDescriptorSet.descriptorCount = 1;

                        m_WriteDescriptorSetPool[descriptorWritesCount] = writeDescriptorSet;
                        imageIndex++;
                        descriptorWritesCount++;
                    } else {
                        // TODO: Don't use buffer members use a single one for the entire uniform buffer
                        //for (sz i = 0; i < descriptor.uboMembers.size(); i++) {
                        auto uboresource = RZResourceManager::Get().getUniformBufferResource(descriptor.uniformBuffer);
                        auto buffer      = static_cast<VKUniformBuffer*>(uboresource);
                        if (buffer) {
                            m_BufferInfoPool[index].buffer = buffer->getBuffer();
                            m_BufferInfoPool[index].offset = descriptor.offset;
                            m_BufferInfoPool[index].range  = buffer->getSize();
                        } else {
                            m_BufferInfoPool[index].buffer = VK_NULL_HANDLE;
                            m_BufferInfoPool[index].offset = 0;
                            m_BufferInfoPool[index].range  = VK_WHOLE_SIZE;
                        }
                        //}

                        VkWriteDescriptorSet writeDescriptorSet{};
                        writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writeDescriptorSet.dstSet          = m_DescriptorSet;
                        writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        writeDescriptorSet.dstBinding      = descriptor.bindingInfo.location.binding;
                        writeDescriptorSet.pBufferInfo     = &m_BufferInfoPool[index];
                        writeDescriptorSet.descriptorCount = 1;

                        m_WriteDescriptorSetPool[descriptorWritesCount] = writeDescriptorSet;
                        index++;
                        descriptorWritesCount++;
                    }
                }
            }
            vkUpdateDescriptorSets(VKDevice::Get().getDevice(), descriptorWritesCount, m_WriteDescriptorSetPool, 0, nullptr);
        }

        void VKDescriptorSet::Destroy() const
        {
            vkDestroyDescriptorSetLayout(VKDevice::Get().getDevice(), setLayout, nullptr);
        }
    }    // namespace Graphics
}    // namespace Razix