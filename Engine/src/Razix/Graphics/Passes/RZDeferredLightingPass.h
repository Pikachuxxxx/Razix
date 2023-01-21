#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/Passes/Data/SceneColorData.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

namespace Razix {
    namespace Graphics {
        class RZDeferredLightingPass : public IRZPass
        {
        public:
            RZDeferredLightingPass()  = default;
            ~RZDeferredLightingPass() = default;

            void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings) override;
            void destroy() override;

        private:
        };
    }    // namespace Graphics
}    // namespace Razix
