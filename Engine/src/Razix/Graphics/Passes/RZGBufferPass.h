#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/Passes/Data/GBufferData.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

namespace Razix {
    namespace Graphics {

        /**
         * Renders the scene onto a GBuffer (pos, normals, depth, metallic, specular etc)
         */
        class RZGBufferPass : public IRZPass
        {
        public:
            RZGBufferPass() = default;
            ~RZGBufferPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings) override;

            void destroy() override;

        private:
            RZDescriptorSet* m_FrameDataSet = nullptr;
            RZPipeline*      m_Pipeline;
            RZCommandBuffer* m_CmdBuffers[RAZIX_MAX_SWAP_IMAGES_COUNT];
        };
    }    // namespace Graphics
}    // namespace Razix
