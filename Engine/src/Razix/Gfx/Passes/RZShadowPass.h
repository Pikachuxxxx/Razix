#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

#include "Razix/Gfx/Passes/Data/GlobalData.h"

namespace Razix {
    namespace Gfx {

        struct LightVPUBOData
        {
            glm::mat4 lightViewProj;
        };

        struct OmniDirectionalShadowPassData
        {
            FrameGraph::RZFrameGraphResource shadowMap;       /* Cube Depth texture to store the shadow map data                    */
            FrameGraph::RZFrameGraphResource lightVPMatrices; /* The light view proj matrices to look at all sides of the cubemap   */
        };

        // TODO: Implement this https://www.fabiensanglard.net/shadowmappingPCF/index.php

        class RZShadowPass : public IRZPass
        {
        public:
            void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
        };
    }    // namespace Gfx
}    // namespace Razix
