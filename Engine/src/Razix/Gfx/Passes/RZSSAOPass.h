#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

#define SSAO_KERNEL_SIZE 64
#define SSAO_NOISE_DIM   4

        struct SSAOParamsData
        {
            float     radius           = 1.0f;
            float     bias             = 0.025f;
            glm::vec2 resolution       = {};
            glm::vec3 camViewPos       = {};
            float     _padding         = 0.0f;
            glm::mat4 viewMatrix       = {};
            glm::mat4 projectionMatrix = {};
        };

        class RZSSAOPass : public IRZPass
        {
        public:
            RZSSAOPass() {}
            ~RZSSAOPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_PreBlurPipeline;
        };
    }    // namespace Gfx
}    // namespace Razix
