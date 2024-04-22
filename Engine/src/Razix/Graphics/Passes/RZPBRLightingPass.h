#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;

        // FIXME: The dot artifact is due to Filtering Mode stuff, mips gen + accessing in pre-filtering stage, specular IBL stuff needs to be fixed properly!

        class RZPBRLightingPass : public IRZPass
        {
        public:
            RZPBRLightingPass() {}
            ~RZPBRLightingPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph,  Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
        };
    }    // namespace Graphics
}    // namespace Razix
