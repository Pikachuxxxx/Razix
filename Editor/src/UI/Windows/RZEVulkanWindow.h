#pragma once

#include <QVulkanWindow>

namespace Razix {
    namespace Editor {

        class RZEVulkanWindow : public QWindow
        {
            Q_OBJECT

        public:
            bool isExposed = false;
            RZEVulkanWindow(QWindow* parentWindow = nullptr);
            ~RZEVulkanWindow();

            void Init();

            QVulkanInstance& getQVKInstance() { return m_QVKInstance; }

        private:
            QVulkanInstance m_QVKInstance;
            VkSurfaceKHR    m_vkSurface = VK_NULL_HANDLE;
        };
    }    // namespace Editor
}    // namespace Razix