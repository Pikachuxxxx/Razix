#pragma once

#include "Razix/Core/RZCore.h"

#include "Razix/Gfx/FrameGraph/RZBlackboard.h"
#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"

namespace Razix {

    class RZScene;

    namespace Gfx {

        struct RZRendererSettings;
        class RAZIX_API IRZPass
        {
        public:
            IRZPass()          = default;
            virtual ~IRZPass() = default;

            // TODO: make RZFrameGraph& a pointer too
            // TODO: make RZRendererSettings a const pointer
            // TODO: make RZScene a const pointer
            virtual void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) = 0;

            virtual void destroy() = 0;
        };
    }    // namespace Gfx
}    // namespace Razix