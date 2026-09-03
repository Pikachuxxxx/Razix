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

            virtual void addPass(RZFrameGraph& framegraph) = 0;
        };
    }    // namespace Gfx
}    // namespace Razix
