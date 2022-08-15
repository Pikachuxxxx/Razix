#pragma once

#include "Razix/Graphics/Renderers/IRZRenderer.h"

#include "Razix/Utilities/RZTimestep.h"

namespace Razix {
    namespace Graphics {

        class RZRenderStack
        {
        public:
            RZRenderStack() {}
            ~RZRenderStack() {}

            void PushRenderer(Graphics::IRZRenderer* renderer);
            void PushRenderer(Graphics::IRZRenderer* renderer, int priorityIndex);

            void PopRenderer(Graphics::IRZRenderer* renderer);

            void BeginScene(RZScene* scene);
            void EndScene(RZScene* scene);

            void OnUpdate(RZTimestep& dt);
            void OnRender();
            void OnResize(uint32_t width, uint32_t height);

            void enableDebugRenderer(bool enable);
            void sortRenderers();

        private:
            std::vector<IRZRenderer*> m_RenderersStack;
        };
    }    // namespace Graphics
}    // namespace Razix
