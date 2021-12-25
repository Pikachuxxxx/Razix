#include "rzxpch.h"
#include "VKDescriptorSet.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKSwapchain.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix {
    namespace Graphics {

        VKDescriptorSet::VKDescriptorSet(const std::vector<RZDescriptor>& descriptors)
            : m_DescriptorPool(VK_NULL_HANDLE)
        {
            // Descriptor sets can't be created directly, they must be allocated from a pool like command buffers i.e. use a descriptor pool to allocate the descriptor sets
            // We first need to describe which descriptor types our descriptor sets are going to contain and how many of them, we allocate a pool for each type of descriptor
            // So we hardcode to create only 2 pools as of now, one for UNIFOR_BUFFER and other for IMAGE_SAMPLER, they can allocate a max of 3 sets so as to facilitate triple buffering
            std::vector<VkDescriptorPoolSize> poolSizes;
            for (size_t i = 0; i < 2; i++) {
                VkDescriptorPoolSize poolSize;
                poolSize.descriptorCount = RAZIX_MAX_SWAP_IMAGES_COUNT;
                poolSize.type = i == 0 ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                poolSizes.push_back(poolSize);
            }

            // Create the pool for each frame with the type and number of pools respectively
            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount  = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes     = poolSizes.data();
            poolInfo.maxSets        = static_cast<uint32_t>(RAZIX_MAX_SWAP_IMAGES_COUNT);

            if (VK_CHECK_RESULT(vkCreateDescriptorPool(VKDevice::Get().getDevice(), &poolInfo, nullptr, &m_DescriptorPool)))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create descriptor Pool!");
            else RAZIX_CORE_TRACE("[Vulkan] Successfully created descriptor Pool");
        }

        VKDescriptorSet::~VKDescriptorSet() { }

        void VKDescriptorSet::UpdateSet(const std::vector<RZDescriptor>& descriptors)
        {
           
        }

    }
}