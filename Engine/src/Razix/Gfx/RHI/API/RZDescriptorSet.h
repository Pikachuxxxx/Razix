#pragma once

#include "Razix/Core/RZDebugConfig.h"

#include "Razix/Gfx/GfxData.h"
#include "Razix/Gfx/Resources/IRZResource.h"

namespace Razix {
    namespace Gfx {

        /* Encapsulating the descriptors of a set along with the setID */
        using DescriptorsPerHeapMap = std::map<u32, std::vector<RZDescriptor>>;
        using DescriptorSets        = std::vector<Gfx::RZDescriptorSetHandle>;    // vector IDx == set Idx

        // https://www.reddit.com/r/vulkan/comments/ybmld8/how_expensive_is_descriptor_set_creationupdate/
        // https://gist.github.com/nanokatze/bb03a486571e13a7b6a8709368bd87cf
        // https://github.com/ARM-software/vulkan_best_practice_for_mobile_developers/blob/master/samples/performance/descriptor_management/descriptor_management_tutorial.md
        // [Transient Resource System] https://logins.github.io/graphics/2021/05/31/RenderGraphs.html#:~:text=Transient%20Resource%20System,are%20also%20called%20transient%20resources.

        /* Shader pointer kind of variable that refers to a bunch of buffers or an image resources and their layout/binding information */
        class RAZIX_API RZDescriptorSet : public IRZResource<RZDescriptorSet>
        {
        public:
            RZDescriptorSet() = default;
            RAZIX_VIRTUAL_DESCTURCTOR(RZDescriptorSet);

            GET_INSTANCE_SIZE;

            /* Updates the descriptor set with the given descriptors */
            virtual void UpdateSet(const std::vector<RZDescriptor>& descriptors) = 0;

            RAZIX_INLINE u32  getSetIdx() const { return m_Desc.setIdx; }
            RAZIX_INLINE void setSetIdx(u32 idx) { m_Desc.setIdx = idx; }

        protected:
            RZDescriptorSetDesc m_Desc;

        private:
            /**
             * Creates the Razix Descriptor set with the given descriptors, it encapsulates the resource per set for all the shader stages
             * @note : As the name suggest the descriptor set contains a set of descriptor resources, along with the data and their binding information
             * 
             * @param descriptor The list of descriptor resources that will be uploaded by the set to various shader stages
             */
            static void Create(void* where, const RZDescriptorSetDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            // only resource manager can create an instance of this class
            friend class RZResourceManager;
        };
    }    // namespace Gfx
}    // namespace Razix
