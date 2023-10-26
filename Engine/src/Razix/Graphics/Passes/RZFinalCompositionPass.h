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
        class RZFinalCompositionPass final : public IRZPass
        {
        public:
            RZFinalCompositionPass() = default;
            ~RZFinalCompositionPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, RZScene* scene, RZRendererSettings& settings) override;
            void destroy() override;

            RAZIX_INLINE void setTonemapMode(TonemapMode mode) { m_TonemapMode = mode; }

        private:
            RZMesh*                    m_ScreenQuadMesh = nullptr;
            RZPipelineHandle           m_Pipeline;
            Graphics::RZDescriptorSet* m_DescriptorSets;
            bool                       updatedRT = false;
            TonemapMode                m_TonemapMode;
        };
    }    // namespace Graphics
}    // namespace Razix