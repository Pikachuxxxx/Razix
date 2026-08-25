#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

#include "Razix/Gfx/Passes/Data/GlobalData.h"

#include "Razix/Gfx/Renderers/RZSystemBinding.h"

namespace Razix {
    namespace Gfx {

        class RZGeomShadersCubeTestPass final : public IRZPass
        {
        public:
            void addPass(FrameGraph::RZFrameGraph& framegraph, const RZWorld* world) override;
            void destroy();

        private:
            RZPipelineHandle m_Pipeline;
        };
    }    // namespace Gfx
}    // namespace Razix
