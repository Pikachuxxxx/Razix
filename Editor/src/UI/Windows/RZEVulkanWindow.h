#pragma once

#include <QMouseEvent>
#include <QVulkanWindow>

#include "RZENativeWindow.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Events/ApplicationEvent.h"

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

            void InitRZWindow();

            bool eventFilter(QObject* watched, QEvent* event)
            {
                // We need to check for both types of mouse release, because it can vary on which type happens when resizing.
                if ((event->type() == QEvent::MouseButtonRelease) || (event->type() == QEvent::NonClientAreaMouseButtonRelease)) {
                    QMouseEvent* pMouseEvent = dynamic_cast<QMouseEvent*>(event);
                    if ((pMouseEvent->button() == Qt::MouseButton::LeftButton) && m_UserIsResizing) {
                        printf("Gotcha!\n");
                        m_UserIsResizing = false;    // reset user resizing flag

                        auto& callback = m_RZWindow->getEventCallbackFunc();

                        m_RZWindow->setWidth(this->width());
                        m_RZWindow->setHeight(this->height());

                        RZWindowResizeEvent e(this->width(), this->height());
                        callback(e);
                    }
                }
                return QObject::eventFilter(watched, event);    // pass it on without eating it
            }

            void resizeEvent(QResizeEvent* event)
            {
                // Now stop the other thread first from rendering before we issue resize commands
                std::lock_guard<std::mutex> lk(RZApplication::m);
                RZApplication::ready_for_execution = false;
                std::cout << "Triggering worker thread to halt execution :::: " << std::endl;
                RZApplication::halt_execution.notify_one();

                std::cout << "Resizing..."
                          << " Width : " << this->width() << " , Height : " << this->height() << std::endl;
                // override from QWidget that triggers whenever the user resizes the window
                m_UserIsResizing = true;
            }

            void mouseMoveEvent(QMouseEvent* event)
            {
                auto& callback = m_RZWindow->getEventCallbackFunc();

                RZMouseMovedEvent e(event->pos().x(), event->pos().y());
                callback(e);
            }

            void mousePressEvent(QMouseEvent* event)
            {
                auto& callback = m_RZWindow->getEventCallbackFunc();

                RZMouseButtonPressedEvent e(event->button());
                callback(e);
            }

            void mouseReleaseEvent(QMouseEvent* event)
            {
                auto& callback = m_RZWindow->getEventCallbackFunc();

                RZMouseButtonReleasedEvent e(event->button());
                callback(e);
            }

            QVulkanInstance& getQVKInstance() { return m_QVKInstance; }
            RZENativeWindow* getRZNativeWindow() { return m_RZWindow; }

        private:
            QVulkanInstance  m_QVKInstance;
            VkSurfaceKHR     m_vkSurface      = VK_NULL_HANDLE;
            bool             m_UserIsResizing = false;
            RZENativeWindow* m_RZWindow;
        };
    }    // namespace Editor
}    // namespace Razix