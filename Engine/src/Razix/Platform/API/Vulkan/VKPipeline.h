#pragma once

#include "Razix/Graphics/API/RZPipeline.h"

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        class VKPipeline : public RZPipeline
        {
        public:
            VKPipeline (const PipelineInfo& pipelineInfo);
            ~VKPipeline () {}

            void Bind (RZCommandBuffer* commandBuffer) override;
            void Destroy () override;

            void init (const PipelineInfo& pipelineInfo);

            inline const VkPipeline&       getPipeline () const { return m_Pipeline; }
            inline const VkPipelineLayout& getPipelineLayout () const { return m_PipelineLayout; }

        private:
            RZShader*        m_Shader;
            VkPipelineLayout m_PipelineLayout;
            VkPipeline       m_Pipeline;
        };
    }    // namespace Graphics
}    // namespace Razix
