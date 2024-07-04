#pragma once

#include "Razix/Graphics/RHI/API/RZPipeline.h"

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        class VKPipeline : public RZPipeline
        {
        public:
            VKPipeline(const RZPipelineDesc& pipelineInfo RZ_DEBUG_NAME_TAG_E_ARG);
            ~VKPipeline() {}

            RAZIX_CLEANUP_RESOURCE

            void Bind(RZDrawCommandBufferHandle cmdBuffer) override;

            void init(const RZPipelineDesc& pipelineInfo RZ_DEBUG_NAME_TAG_E_ARG);

            inline const VkPipeline&       getPipeline() const { return m_Pipeline; }
            inline const VkPipelineLayout& getPipelineLayout() const { return m_PipelineLayout; }

        private:
            VkPipelineLayout m_PipelineLayout;
            VkPipeline       m_Pipeline;
        };
    }    // namespace Graphics
}    // namespace Razix
