#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;

        class RZSkyboxPass : public IRZPass
        {
        public:
            RZSkyboxPass() {}
            ~RZSkyboxPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings) override;

            void destroy() override;

            void useProceduralSkybox(bool use) { m_UseProceduralSkybox = use; }

        private:
            RZPipeline*                   m_Pipeline;
            RZPipeline*                   m_ProceduralPipeline;
            std::vector<RZCommandBuffer*> m_CommandBuffers;
            RZDescriptorSet*              m_FrameDataDescriptorSet;
            RZDescriptorSet*              m_LightProbesDescriptorSet;
            RZMesh*                       m_SkyboxCube;
            bool                          m_UseProceduralSkybox = true;
        };
    }    // namespace Graphics
}    // namespace Razix
