#pragma once

#include "Razix/Graphics/Renderers/RZWorldRenderer.h"

#include "Razix/Core/System/IRZSystem.h"

namespace Razix {
    namespace Graphics {

        class RAZIX_API RZRenderSystem : public Razix::IRZSystem<RZRenderSystem>
        {
        public:
            RZRenderSystem() {}
            ~RZRenderSystem() {}

            void StartUp() override;
            void Shutdown() override;

            void OnUpdate(const RZTimestep& dt) override;
        };

    }    // namespace Graphics
}    // namespace Razix