// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZEVulkanWindow.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"

namespace Razix {
    namespace Editor {
        RZEVulkanWindow::RZEVulkanWindow(QWindow* parentWindow)
            : QWindow(parentWindow)    
        {
            setSurfaceType(VulkanSurface);
        }

        RZEVulkanWindow::~RZEVulkanWindow()
        {}

        void RZEVulkanWindow::Init()
        {
            Razix::Graphics::VKContext* context = static_cast<Razix::Graphics::VKContext*>(Razix::Graphics::RZGraphicsContext::GetContext());

            m_QVKInstance.setVkInstance(context->getInstance());
            auto result = m_QVKInstance.create();

            this->setVulkanInstance(&m_QVKInstance);

            auto version = m_QVKInstance.apiVersion();

            sInstance = this;
        }

        void RZEVulkanWindow::InitRZWindow()
        {
            WindowProperties props{};
            props.Title  = "Vulkan Viewport";
            props.Width  = this->width();
            props.Height = this->height();

            // TODO: make this platform agnostic
            HWND hwnd  = (HWND) this->winId();
            m_RZWindow = new RZENativeWindow(&hwnd, props);
        }

        bool RZEVulkanWindow::IsKeyPressedImpl(int keycode)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        bool RZEVulkanWindow::IsKeyReleasedImpl(int keycode)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        bool RZEVulkanWindow::IsIsKeyHeldImpl(int keycode)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        bool RZEVulkanWindow::IsMouseButtonPressedImpl(int button)
        {
            if (button == m_MouseButton - 1)
                return true;
            else
                return false;
        }

        bool RZEVulkanWindow::IsMouseButtonReleasedImpl(int button)
        {
            if (button == m_MouseButton - 1)
                return true;
            else
                return false;
        }

        bool RZEVulkanWindow::IsMouseButtonHeldImpl(int button)
        {
            if (button == m_MouseButton - 1)
                return true;
            else
                return false;
        }

        std::pair<float, float> RZEVulkanWindow::GetMousePositionImpl()
        {
            return std::pair<float, float>(m_MousePos.x, m_MousePos.y);
        }

        float RZEVulkanWindow::GetMouseXImpl()
        {
            return m_MousePos.x;
        }

        float RZEVulkanWindow::GetMouseYImpl()
        {
            return m_MousePos.y;
        }

    }    // namespace Editor
}    // namespace Razix