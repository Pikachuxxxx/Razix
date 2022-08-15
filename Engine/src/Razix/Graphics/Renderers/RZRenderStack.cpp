// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZRenderStack.h"

#include "Razix/Graphics/API/RZAPIRenderer.h"
#include "Razix/Graphics/API/RZSwapchain.h"

namespace Razix {
    namespace Graphics {

        void RZRenderStack::PushRenderer(Graphics::IRZRenderer* renderer)
        {
            m_RenderersStack.push_back(renderer);
        }

        void RZRenderStack::PushRenderer(Graphics::IRZRenderer* renderer, int priorityIndex)
        {
            renderer->setRenderPriorityIndex(priorityIndex);
            m_RenderersStack.push_back(renderer);
        }

        void RZRenderStack::PopRenderer(Graphics::IRZRenderer* renderer)
        {
            m_RenderersStack.erase(std::remove(m_RenderersStack.begin(), m_RenderersStack.end(), renderer), m_RenderersStack.end());
        }

        void RZRenderStack::BeginScene(RZScene* scene)
        {
            // TODO: enable the debug renderer separately here
            for (auto renderer: m_RenderersStack)
                renderer->BeginScene(scene);
        }

        void RZRenderStack::EndScene(RZScene* scene)
        {
            for (auto renderer: m_RenderersStack)
                renderer->EndScene(scene);
        }

        void RZRenderStack::OnUpdate(RZTimestep& dt)
        {
        }

        void RZRenderStack::OnRender()
        {
            for (auto renderer: m_RenderersStack) {
                renderer->Begin();

                renderer->Submit(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                renderer->End();

                renderer->Present();
            }
        }

        void RZRenderStack::OnResize(uint32_t width, uint32_t height)
        {
            for (auto renderer: m_RenderersStack)
                renderer->Resize(width, height);
        }

        void RZRenderStack::enableDebugRenderer(bool enable)
        {
        }

        void RZRenderStack::sortRenderers()
        {
        }

    }    // namespace Graphics
}    // namespace Razix