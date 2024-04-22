#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

namespace Razix {
    namespace Graphics {

        enum TonemapMode : u32;
        class RZMesh;
        class RZDescriptorSet;

        struct CompositeData
        {
            FrameGraph::RZFrameGraphResource presentationTarget;    // Not an actual resource unlike RTs or Textures
            FrameGraph::RZFrameGraphResource depthTexture;
        };

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