#pragma once

#include <QCloseEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QVulkanWindow>

#include "RZENativeWindow.h"
#include "RZESceneHierarchyPanel.h"

#include "Razix/Core/OS/RZInput.h"
#include "Razix/Core/App/RZApplication.h"
#include "Razix/Events/ApplicationEvent.h"

namespace Razix {
    namespace Editor {

        class RZEVulkanWindow : public QWindow, public Razix::RZInput
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
                        m_UserIsResizing = false;    // reset user resizing flag

                        if (!m_RZWindow)
                            return false;

                        auto& callback = m_RZWindow->getEventCallbackFunc();

                        m_RZWindow->setWidth(this->width());
                        m_RZWindow->setHeight(this->height());

                        RZWindowResizeEvent e(this->width(), this->height());
                        if (callback)
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
                //RAZIX_INFO("Triggering worker thread to halt execution ::::");
                RZApplication::halt_execution.notify_one();

                // override from QWidget that triggers whenever the user resizes the window
                m_UserIsResizing = true;

                this->requestUpdate();
            }

            void mouseMoveEvent(QMouseEvent* event)
            {
                m_MousePos = glm::vec2(event->pos().x(), event->pos().y());

                if (!m_RZWindow)
                    return;

                auto& callback = m_RZWindow->getEventCallbackFunc();

                RZMouseMovedEvent e(event->pos().x(), event->pos().y());
                if (callback)
                    callback(e);
            }

            void mousePressEvent(QMouseEvent* event)
            {
                auto& callback        = m_RZWindow->getEventCallbackFunc();
                m_MouseReleasedButton = -1;

                if (!m_RZWindow)
                    return;

                RZMouseButtonPressedEvent e(event->button());
                m_MousePressDirty = true;

                m_MousePressedButton = event->button();
                if (callback)
                    callback(e);

                    // Entity selection
#if 0
                int32_t selectedEntity = Razix::RZEngine::Get().getRenderStack().getSelectedEntityID();
                // Find the entity from the registry
                Razix::RZScene* scene = RZEngine::Get().getSceneManager().getCurrentScene();
                if (!scene)
                    return;
                auto& registry = scene->getRegistry();

                registry.each([&](auto& entity) {
                    if (registry.valid(entity)) {
                        if (selectedEntity != -1 && entity == entt::entity(selectedEntity)) {
                            RZEntity rzEntity(entity, scene);
                            RZApplication::Get().setGuzimoForEntity(rzEntity);
                            emit OnEntitySelected(rzEntity);
                            return;
                        }
                        // TODO: use ui.sceneTree->clearSelection() if we don't select on any entity
                    }
                });
#endif
            }

            void mouseReleaseEvent(QMouseEvent* event)
            {
                if (!m_RZWindow)
                    return;

                auto& callback       = m_RZWindow->getEventCallbackFunc();
                m_MousePressedButton = -1;
                RZMouseButtonReleasedEvent e(event->button());
                m_MouseReleaseDirty   = true;
                m_MouseReleasedButton = event->button();
                if (callback)
                    callback(e);
            }

            void keyPressEvent(QKeyEvent* event)
            {
                if (!m_RZWindow)
                    return;

                auto& callback = m_RZWindow->getEventCallbackFunc();
                m_KeyReleased  = -1;
                m_KeyPressed   = event->key();
                RZKeyPressedEvent e(event->key(), 1);
                if (callback)
                    callback(e);
            }

            void keyReleaseEvent(QKeyEvent* event)
            {
                if (!m_RZWindow)
                    return;

                auto& callback = m_RZWindow->getEventCallbackFunc();
                m_KeyPressed   = -1;
                m_KeyReleased  = event->key();
                RZKeyReleasedEvent e(event->key());
                if (callback)
                    callback(e);
            }

            void closeEvent(QCloseEvent* event)
            {
                // TODO: Engine should run despite not having a surface to render (without falling back to offline rendering)
                event->ignore();
            }

            QVulkanInstance& getQVKInstance() { return m_QVKInstance; }
            RZENativeWindow* getRZNativeWindow() { return m_RZWindow; }

        signals:
            void OnEntitySelected(RZEntity entity);

        private:
            QVulkanInstance  m_QVKInstance;
            VkSurfaceKHR     m_vkSurface      = VK_NULL_HANDLE;
            bool             m_UserIsResizing = false;
            RZENativeWindow* m_RZWindow       = nullptr;
            glm::vec2        m_MousePos;
            int              m_MousePressedButton  = -1;
            int              m_MouseReleasedButton = -1;
            int              m_KeyPressed          = -1;
            int              m_KeyReleased         = -1;
            bool             m_MousePressDirty;
            bool             m_MouseReleaseDirty;

        protected:
            bool IsKeyPressedImpl(int keycode) override;
            bool IsKeyReleasedImpl(int keycode) override;
            bool IsIsKeyHeldImpl(int keycode) override;
            bool IsMouseButtonPressedImpl(int button) override;
            bool IsMouseButtonReleasedImpl(int button) override;
            bool IsMouseButtonHeldImpl(int button) override;

            float GetMouseXImpl() override;
            float GetMouseYImpl() override;

            std::pair<float, float> GetMousePositionImpl() override;
        };
    }    // namespace Editor
}    // namespace Razix