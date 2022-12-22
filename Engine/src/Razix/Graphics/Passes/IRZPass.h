#pragma once

#include "Razix/Graphics/FrameGraph/RZBlackboard.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"

namespace Razix {
    namespace Graphics {

        class RZCommandBuffer;
        class RZPipeline;
        class RZRenderPass;
        class RZFramebuffer;
        class RZRenderTexture;

        class IRZPass
        {
        public:
            IRZPass()          = default;
            virtual ~IRZPass() = default;

            virtual void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard) = 0;

        protected:
            RZCommandBuffer* m_CmdBuffers[RAZIX_MAX_SWAP_IMAGES_COUNT];
            RZPipeline*      m_Pipeline   = nullptr;
        };
    }    // namespace Graphics
}    // namespace Razix