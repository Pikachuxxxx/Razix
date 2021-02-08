#include "rzxpch.h"
#include "Application.h"

#include "Razix/Events/ApplicationEvent.h"

namespace Razix
{
    Application* Application::sInstance = nullptr;

    Application::Application()
    {
        RZX_CORE_ASSERT(!sInstance, "Application already exists!");
        sInstance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(RZX_BIND_CB_EVENT_FN(OnEvent));
    }

    Application::~Application()
    {

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
            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();
            
            m_Window->OnWindowUpdate();
        }
    }

}
