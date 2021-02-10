#include "rzxpch.h"
#include "Application.h"

#include "Razix/Events/ApplicationEvent.h"

#include <glad/glad.h>

namespace Razix
{
    Application* Application::sInstance = nullptr;

    Application::Application()
    {
        RZX_CORE_ASSERT(!sInstance, "Application already exists!");
        sInstance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(RZX_BIND_CB_EVENT_FN(OnEvent));
        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
        // TODO: Detatch the layers explicitly here if not previously detached elsewhere 
    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(RZX_BIND_CB_EVENT_FN(OnWindowClose));

        // Core Event tracing
        RZX_CORE_TRACE("Event: {0}", event);

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            (*--it)->OnEvent(event);
            if(event.Handled)
                break;
        }
    }
 
    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    void Application::Run()
    {
        while (m_Running)
        {
           glClear(GL_COLOR_BUFFER_BIT);

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();

#if 1
            // TODO: Pass this to the render pass of the renderer
            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack)
                layer->OnImguiRender();
            m_ImGuiLayer->End();
#endif

            m_Window->OnWindowUpdate();
        }
    }

}
