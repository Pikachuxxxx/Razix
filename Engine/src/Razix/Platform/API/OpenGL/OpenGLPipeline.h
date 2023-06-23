#pragma once

#include "Razix/Graphics/RHI/API/RZPipeline.h"

namespace Razix {
    namespace Graphics {

        class OpenGLPipeline : public RZPipeline
        {
        public:
            OpenGLPipeline(const PipelineDesc& pipelineInfo);
            ~OpenGLPipeline() {}

            void Bind(RZCommandBuffer* commandBuffer) override;
            void Destroy() override;

            RAZIX_INLINE RZShader* getShader() { return m_Shader; } 

        private:
            RZShader*           m_Shader;       /* Store a reference to the shader used by the pipeline */
            const PipelineDesc& m_PipelineInfo; /* Since unlike Vulkan we don't pre-bake pipeline objects we store this until draw time to forever */
        };
    }    // namespace Graphics
}    // namespace Razix
