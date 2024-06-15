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

            void Bind(RZDrawCommandBuffer* commandBuffer) override;

            void init(const RZPipelineDesc& pipelineInfo RZ_DEBUG_NAME_TAG_E_ARG);

            inline const VkPipeline&       getPipeline() const { return m_Pipeline; }
            inline const VkPipelineLayout& getPipelineLayout() const { return m_PipelineLayout; }

            void DestroyResource() override;

        private:
            VkPipelineLayout              m_PipelineLayout;
            VkPipeline                    m_Pipeline;
        };
    }    // namespace Graphics
}    // namespace Razix
