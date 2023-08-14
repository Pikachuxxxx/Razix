#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;

        // FIZME: The dot artifact is due to Filtering Mode make, verify them and they will go away

        class RZPBRLightingPass : public IRZPass
        {
        public:
            RZPBRLightingPass() {}
            ~RZPBRLightingPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings) override;

            void destroy() override;

        private:
            RZPipeline*      m_Pipeline;
            RZDescriptorSet* m_ShadowDataSet;
            RZDescriptorSet* m_PBRDataSet;
            RZDescriptorSet* m_GBufferDataSet;
            RZMesh*          m_ScreenQuadMesh = nullptr;
        };
    }    // namespace Graphics
}    // namespace Razix
