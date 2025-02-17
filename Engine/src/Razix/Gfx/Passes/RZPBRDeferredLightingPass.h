#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

        class RZMesh;

        // FIXME: The dot artifact is due to Filtering Mode stuff, mips gen + accessing in pre-filtering stage, specular IBL stuff needs to be fixed properly!

        class RZPBRDeferredLightingPass : public IRZPass
        {
        public:
            // TODO: Move these to light component settings, all shadow related settings must be under the LightRendererComponent
            f32 biasScale = 0.1f;
            f32 maxBias   = 0.005f;

            void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
        };
    }    // namespace Gfx
}    // namespace Razix
