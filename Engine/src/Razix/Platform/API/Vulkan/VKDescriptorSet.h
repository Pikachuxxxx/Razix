#pragma once

#include "Razix/Graphics/API/RZDescriptorSet.h"

#ifdef RAZIX_RENDER_API_VULKAN

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        /* Vulkan implementation of the descriptor sets, these are used to bind/map the uniform, storage and image resources to shaders */
        class VKDescriptorSet : public RZDescriptorSet
        {
        public:
            VKDescriptorSet();
            ~VKDescriptorSet();

        private:
            VkDescriptorPool                m_DescriptorPool;   /* The descriptor pool from which the descriptor sets will be allocated from                                */
            std::vector<VkDescriptorSet>    m_DescriptorSets;   /* Each frame gets a descriptor set that helps in mapping the necessary data to the shader slots            */

        };

    }
}
#endif