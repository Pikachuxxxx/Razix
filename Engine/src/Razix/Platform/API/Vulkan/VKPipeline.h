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

            void Bind(RZCommandBuffer* commandBuffer) override;
            void Destroy() override;

            void init(const RZPipelineDesc& pipelineInfo RZ_DEBUG_NAME_TAG_E_ARG);

            inline const VkPipeline&       getPipeline() const { return m_Pipeline; }
            inline const VkPipelineLayout& getPipelineLayout() const { return m_PipelineLayout; }

        private:
            RZShader*        m_Shader;
            VkPipelineLayout m_PipelineLayout;
            VkPipeline       m_Pipeline;
        };
    }    // namespace Graphics
}    // namespace Razix
