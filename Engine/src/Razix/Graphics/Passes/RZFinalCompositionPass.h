#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

namespace Razix {
    namespace Graphics {

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
        class RZFinalCompositionPass final : public IRZPass
        {
        public:
            RZFinalCompositionPass() = default;
            ~RZFinalCompositionPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, RZScene* scene, RZRendererSettings& settings) override;

            void destroy() override;

        private:
            RZMesh*                    m_ScreenQuadMesh = nullptr;
            RZPipeline*                m_Pipeline;
        };
    }    // namespace Graphics
}    // namespace Razix