#pragma once

#include "Razix/Gfx/RHI/API/RZDescriptorSet.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Gfx {
        // TODO: Query and fill these based on device props API
    #define MAX_BUFFER_INFOS         32
    #define MAX_COMBINED_IMAGE_INFOS 32
    #define MAX_TEXTURE_INFOS        32
    #define MAX_SAMPLER_INFOS        32
    #define MAX_WRITE_DESCRIPTORS    32 * 4

        /* Vulkan implementation of the descriptor sets, these are used to bind/map the uniform, storage and image resources to shaders */
        class VKDescriptorSet : public RZDescriptorSet
        {
        public:
            VKDescriptorSet(const RZDescriptorSetDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            //---------------------------------------
            /* Releases the IRZResource */
            RAZIX_CLEANUP_RESOURCE
            //---------------------------------------

            void UpdateSet(const std::vector<RZDescriptor>& descriptors) override;

            RAZIX_INLINE const VkDescriptorSet& getDescriptorSet() const { return m_DescriptorSet; }

        private:
            VkDescriptorPool        m_DescriptorPool         = {}; /* The descriptor pool from which the descriptor sets will be allocated from                                */
            VkDescriptorSet         m_DescriptorSet          = {}; /* Each frame gets a descriptor set that helps in mapping the necessary data to the shader slots            */
            VkDescriptorBufferInfo* m_BufferInfoPool         = nullptr;
            VkDescriptorImageInfo*  m_ImageInfoPool          = nullptr;    // Currently contains both CombinedImageSampler/Texture/Sampler descriptors
            VkWriteDescriptorSet*   m_WriteDescriptorSetPool = nullptr;
            VkDescriptorSetLayout   m_SetLayout              = VK_NULL_HANDLE;
            VkSampler               m_DefaultSampler         = VK_NULL_HANDLE;
        };
    }    // namespace Gfx
}    // namespace Razix
#endif