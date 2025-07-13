#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

#define SSAO_KERNEL_SIZE 64
#define SSAO_NOISE_DIM   4

        struct SSAOParamsData
        {
            float    radius           = 1.0f;
            float    bias             = 0.025f;
            float2   resolution       = {};
            float3   camViewPos       = {};
            float    _padding         = 0.0f;
            float4x4 viewMatrix       = {};
            float4x4 projectionMatrix = {};
        };

        class RZSSAOPass : public IRZPass
        {
        public:
            RZSSAOPass() {}
            ~RZSSAOPass() {}

            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_PreBlurPipeline;
        };
    }    // namespace Gfx
}    // namespace Razix
