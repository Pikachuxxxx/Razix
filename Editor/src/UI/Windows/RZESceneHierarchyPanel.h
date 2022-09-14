#pragma once

#include "Razix/Core/RZEngine.h"
#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

#include "generated/ui_RZESceneHierarchyPanel.h"

#include "UI/Windows/RZEMainWindow.h"

#include <QMouseEvent>
#include <QFrame>

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