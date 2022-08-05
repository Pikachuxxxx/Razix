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

    }    // namespace Editor
}    // namespace Razix