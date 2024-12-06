#pragma once

#include "Razix/Graphics/FrameGraph/RZBlackboard.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"

namespace Razix {

    class RZScene;

    namespace Graphics {

        struct RZRendererSettings;

        class RZDrawCommandBuffer;
        class RZPipeline;
        class RZRenderPass;
        class RZFramebuffer;
        class RZRenderTexture;
        class RZUniformBuffer;
        class RZTexture;
        class RZDescriptorSet;

        class IRZPass
        {
        public:
            IRZPass()          = default;
            virtual ~IRZPass() = default;

            virtual void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) = 0;

            virtual void destroy() = 0;
        };
    }    // namespace Graphics
}    // namespace Razix