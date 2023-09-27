#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;

        // FIXME: The dot artifact is due to Filtering Mode stuff, mips gen + accessing in pre-filterring stage, specular IBL stuff needs to be fixed properly!

        class RZPBRLightingPass : public IRZPass
        {
        public:
            RZPBRLightingPass() {}
            ~RZPBRLightingPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph,  Razix::RZScene* scene, RZRendererSettings& settings) override;

            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
            RZUniformBuffer* m_PBRPassBindingUBO;
            RZDescriptorSet* m_PBRBindingSet;
            RZDescriptorSet* m_ShadowDataSet;
            RZDescriptorSet* m_PBRDataSet;

            RZMesh* m_ScreenQuadMesh = nullptr;
        };
    }    // namespace Graphics
}    // namespace Razix
