#pragma once

#include "Razix/Core/RZEngine.h"
#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

#include "generated/ui_RZESceneHierarchyPanel.h"

#include "UI/Windows/RZEMainWindow.h"

#include <QFrame>
#include <QMouseEvent>

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
            void drawEntityNode(RZEntity& entity);

            void keyPressEvent(QKeyEvent* event)
            {
                if (event->key() == Qt::Key::Key_Delete) {
                    QList<QTreeWidgetItem*> selectedItems = ui.sceneTree->selectedItems();
                    if (!selectedItems.size())
                        return;

                    Razix::RZScene* scene    = RZEngine::Get().getSceneManager().getCurrentScene();
                    auto&           registry = scene->getRegistry();

                    for (size_t i = 0; i < selectedItems.size(); i++) {
                        QVariant entityVariant = selectedItems[i]->data(0, Qt::UserRole);
                        auto     entity        = entityVariant.value<RZEntity>();
                        //RZApplication::Get().setGuzimoForEntity(RZEntity(entt::null));
                        registry.destroy(entity);
                    }

                    ui.sceneTree->clear();
                    populateHierarchy();
                    repaint();
                }
            }

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