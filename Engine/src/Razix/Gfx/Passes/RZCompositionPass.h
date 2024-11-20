#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

namespace Razix {
    namespace Gfx {

        enum TonemapMode : u32;
        class RZMesh;
        class RZDescriptorSet;

        /**
         * Final composition pass of all the render targets which will submit to the GPU for presentation
         */
        class RZCompositionPass final : public IRZPass
        {
        public:
            RZCompositionPass() = default;
            ~RZCompositionPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
        };
    }    // namespace Graphics
}    // namespace Razix