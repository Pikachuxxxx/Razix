#include "rzxpch.h"
#include "Application.h"

#include "Razix/Events/ApplicationEvent.h"

namespace Razix
{
    #define BIND_CB_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
    Application::Application()
    {
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_CB_EVENT_FN(OnEvent));

    }

    Application::~Application()
    {

    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_CB_EVENT_FN(OnWindowClose));

        RZX_CORE_TRACE("Event: {0}", event);
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
            m_Window->OnWindowUpdate();
        }
    }


}
