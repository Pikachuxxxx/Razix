#pragma once

#include "Razix/Graphics/API/RZPipeline.h"

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        class VKPipeline : public RZPipeline
        {
        public:
            VKPipeline(const PipelineInfo& pipelineInfo);
            ~VKPipeline();

            void Bind(RZCommandBuffer* commandBuffer) override;

            inline const VkPipeline& getPipeline() const { return m_Pipeline; }

        private:
            Ref<RZShader>       m_Shader;
            VkPipelineLayout    m_PipelineLayout;
            VkPipeline          m_Pipeline;
        };
    }
}

