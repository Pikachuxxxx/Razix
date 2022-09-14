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

            void Destroy();

            void enableDebugRenderer(bool enable);
            void sortRenderers();

            inline const int32_t& getSelectedEntityID() { return m_SelectedEntityID; }

        private:
            std::vector<IRZRenderer*> m_RenderersStack;
            int32_t                   m_SelectedEntityID;
        };
    }    // namespace Graphics
}    // namespace Razix
