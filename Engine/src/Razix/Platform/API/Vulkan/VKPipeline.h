#pragma once

#include "Razix/Gfx/RHI/API/RZPipeline.h"

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Gfx {

        class VKPipeline : public RZPipeline
        {
        public:
            VKPipeline(const RZPipelineDesc& pipelineInfo RZ_DEBUG_NAME_TAG_E_ARG);
            ~VKPipeline() {}

            RAZIX_CLEANUP_RESOURCE

            void Bind(RZDrawCommandBufferHandle cmdBuffer) override;

            void initGraphics(const RZPipelineDesc& pipelineInfo RZ_DEBUG_NAME_TAG_E_ARG);
            void initCompute(const RZPipelineDesc& pipelineInfo RZ_DEBUG_NAME_TAG_E_ARG);

            inline const VkPipeline&       getPipeline() const { return m_Pipeline; }
            inline const VkPipelineLayout& getPipelineLayout() const { return m_PipelineLayout; }

        private:
            VkPipelineLayout m_PipelineLayout;    // TODO: Remove this, replace it with RZRootSignature (from shader FX)
            VkPipeline       m_Pipeline;
        };
    }    // namespace Gfx
}    // namespace Razix
