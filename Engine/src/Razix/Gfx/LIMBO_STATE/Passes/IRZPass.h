#pragma once

#include "Razix/Gfx/FrameGraph/RZBlackboard.h"
#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"

namespace Razix {

    class RZScene;

    namespace Gfx {

        struct RZRendererSettings;

        class RZDrawCommandBuffer;
        class RZPipeline;
        class RZRenderPass;
        class RZFramebuffer;
        class RZRenderTexture;
        class RZUniformBuffer;
        class RZTexture;
        class RZDescriptorSet;

        class RAZIX_API IRZPass
        {
        public:
            IRZPass()          = default;
            virtual ~IRZPass() = default;

            // TODO: make RZFrameGraph& a pointer too
            virtual void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) = 0;

            virtual void destroy() = 0;
        };
    }    // namespace Gfx
}    // namespace Razix