#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

namespace Razix {
    namespace Graphics {

        struct CompositeData
        {
            FrameGraph::RZFrameGraphResource presentationTarget;
            FrameGraph::RZFrameGraphResource depthTexture;
        };

        /**
         * Final composition pass of all the render targets which will submit to the GPU for presentation
         */
        class RZFinalCompositionPass final : public IRZPass
        {
        public:
            RZFinalCompositionPass() = default;
            ~RZFinalCompositionPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, const FrameGraph::RZBlackboard& blackboard) override;

        private:
            void init();
        };
    }    // namespace Graphics
}    // namespace Razix