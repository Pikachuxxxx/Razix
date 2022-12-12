#pragma once

#include <QMouseEvent>
#include <QWidget>

#include "generated/ui_RZEViewport.h"

#include "UI/Windows/RZEVulkanWindow.h"

namespace Razix {
    namespace Editor {
        class RZEViewport : public QWidget
        {
            Q_OBJECT

        public:
            RZEViewport(RZESceneHierarchyPanel* sceneHierarchyPanel, QWidget* parent = nullptr);
            ~RZEViewport();

            bool eventFilter(QObject* watched, QEvent* event)
            {
                // We need to check for both types of mouse release, because it can vary on which type happens when resizing.
                if ((event->type() == QEvent::MouseButtonRelease) || (event->type() == QEvent::NonClientAreaMouseButtonRelease)) {
                    QMouseEvent* pMouseEvent = dynamic_cast<QMouseEvent*>(event);
                    if ((pMouseEvent->button() == Qt::MouseButton::LeftButton) && m_UserIsResizing) {
                        printf("Gotcha!\n");
                        m_UserIsResizing = false;    // reset user resizing flag
                    }
                }
                return QObject::eventFilter(watched, event);    // pass it on without eating it
            }

            void resizeEvent(QResizeEvent* event)
            {
                // override from QWidget that triggers whenever the user resizes the window
                m_UserIsResizing = true;
            }

            inline RZEVulkanWindow* getVulkanWindow() { return m_VulkanViewportWindow; }

        private:
            Ui::Viewport     ui;
            bool             m_UserIsResizing = false;
            RZEVulkanWindow* m_VulkanViewportWindow;
        };
    }    // namespace Editor
}    // namespace Razix