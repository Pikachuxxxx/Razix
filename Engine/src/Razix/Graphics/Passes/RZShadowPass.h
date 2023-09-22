#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/Passes/Data/GlobalData.h"

namespace Razix {
    namespace Graphics {

        struct LightVPUBOData
        {
            glm::mat4 lightViewProj;
        };

        struct OmniDirectionalShadowPassData
        {
            FrameGraph::RZFrameGraphResource shadowMap;       /* Cube Depth texture to store the shadow map data                    */
            FrameGraph::RZFrameGraphResource lightVPMatrices; /* The light view proj matrices to look at all sides of the cubemap   */
        };

        constexpr u32 kShadowMapSize = 4096;

        class RZShadowPass : public IRZPass
        {
        public:
            RZShadowPass()  = default;
            ~RZShadowPass() = default;

            void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings) override;

            void destroy() override;

        private:
            RZUniformBufferHandle m_LightViewProjUBO;
            RZDescriptorSet*      m_LVPSet;
            RZPipelineHandle      m_Pipeline;
        };
    }    // namespace Graphics
}    // namespace Razix
