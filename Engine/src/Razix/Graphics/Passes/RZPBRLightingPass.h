#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

namespace Razix {
    namespace Graphics {

        class RZPBRLightingPass : public IRZPass
        {
        public:
            RZPBRLightingPass() {}
            ~RZPBRLightingPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings) override;

            void destroy() override;

        private:
            RZPipeline*                   m_Pipeline;
            std::vector<RZCommandBuffer*> m_CommandBuffers;
            RZDescriptorSet*              m_FrameDataDescriptorSet;
            RZDescriptorSet*              m_SceneLightsDataDescriptorSet;
        };
    }    // namespace Graphics
}    // namespace Razix
