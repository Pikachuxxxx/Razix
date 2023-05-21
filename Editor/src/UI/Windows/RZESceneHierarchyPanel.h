#pragma once

#include "generated/ui_RZESceneHierarchyPanel.h"

#include "UI/Windows/RZEMainWindow.h"

#include <QFrame>
#include <QMouseEvent>

#include "Razix/Core/RZEngine.h"

namespace Razix {
    class RZEntity;
}

Q_DECLARE_METATYPE(Razix::RZEntity);

namespace Razix {
    namespace Editor {
        class RZESceneHierarchyPanel : public QFrame
        {
            Q_OBJECT
        public:
            RZESceneHierarchyPanel(RZEMainWindow* mainWindow, QWidget* parent = nullptr);
            ~RZESceneHierarchyPanel();

            void populateHierarchy();
            void drawEntityNode(RZEntity& entity, RZScene* scene, entt::registry& registry, QTreeWidgetItem* parentItem);

            void keyPressEvent(QKeyEvent* event)
            {
#if 1
                if (event->key() == Qt::Key::Key_Delete) {
                    QList<QTreeWidgetItem*> selectedItems = ui.sceneTree->selectedItems();
                    if (!selectedItems.size())
                        return;

                    Razix::RZScene* scene    = Razix::RZEngine::Get().getSceneManager().getCurrentScene();
                    auto&           registry = scene->getRegistry();

                    for (size_t i = 0; i < selectedItems.size(); i++) {
                        RZApplication::Get().disableGuizmoEditing();
                        QVariant entityVariant = selectedItems[i]->data(0, Qt::UserRole);
                        auto     entity        = entityVariant.value<RZEntity>();

                        DestroyEntity(entity, registry);
                    }

                    ui.sceneTree->clear();
                    populateHierarchy();
                    repaint();
                }
#endif
            }

            void DestroyEntity(entt::entity entity, entt::registry& registry);

        signals:
            void OnEntitySelected(RZEntity entity);

        public slots:
            void OnItemSelected();
            void OnEntitySelectedByUser(RZEntity entity);
            void UpdatePanel();
            //void ReDrawUI() { repaint(); }

        private:
            Ui::SceneHierarchyPanel ui;
            QHeaderView*            header;
            RZEMainWindow*          m_MainWindow;
        };
    }    // namespace Editor
}    // namespace Razix