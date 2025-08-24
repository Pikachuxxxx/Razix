#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

        class RZMesh;

        // FIXME: The dot artifact is due to Filtering Mode stuff, mips gen + accessing in pre-filtering stage, specular IBL stuff needs to be fixed properly!

        class RZPBRDeferredShadingPass : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
        };
    }    // namespace Gfx
}    // namespace Razix
