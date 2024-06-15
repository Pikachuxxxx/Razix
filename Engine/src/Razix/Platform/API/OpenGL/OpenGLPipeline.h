#pragma once

#include "Razix/Graphics/RHI/API/RZPipeline.h"

namespace Razix {
    namespace Graphics {

        class OpenGLPipeline : public RZPipeline
        {
        public:
            OpenGLPipeline(const RZPipelineDesc& pipelineInfo);
            ~OpenGLPipeline() {}

            void Bind(RZDrawCommandBuffer* commandBuffer) override;

            RAZIX_INLINE RZShaderHandle getShaderHandle() { return m_Desc.shader; }

            void DestroyResource() override;
        };
    }    // namespace Graphics
}    // namespace Razix
