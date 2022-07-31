#include "RZEVulkanWindow.h"
#include "rzxpch.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"

namespace Razix {
    namespace Editor {
        RZEVulkanWindow::RZEVulkanWindow(QWindow* parentWindow)
            : QWindow(parentWindow)
        {
            setSurfaceType(VulkanSurface);

            Razix::Graphics::VKContext* context = static_cast<Razix::Graphics::VKContext*>(Razix::Graphics::RZGraphicsContext::GetContext());

            m_QVKInstance.setVkInstance(context->getInstance());
            auto result = m_QVKInstance.create();

            this->setVulkanInstance(&m_QVKInstance);

           auto version = m_QVKInstance.apiVersion();
        }

        RZEVulkanWindow::~RZEVulkanWindow()
        {}
    }    // namespace Editor
}    // namespace Razix