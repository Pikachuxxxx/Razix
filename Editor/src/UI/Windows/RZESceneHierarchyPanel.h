#pragma once

#include "Razix/Core/RZEngine.h"
#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

#include "generated/ui_RZESceneHierarchyPanel.h"

#include <QFrame>

Q_DECLARE_METATYPE(Razix::RZEntity);

namespace Razix {
    namespace Editor {
        class RZESceneHierarchyPanel : public QFrame
        {
            Q_OBJECT
        public:
            RZESceneHierarchyPanel(QWidget* parent = nullptr);
            ~RZESceneHierarchyPanel();

            void populateHierarchy();
            void drawEntityNode(RZEntity& entity);

        signals:
            void OnEntitySelected(RZEntity entity);

        public slots:
            void OnItemSelected();

        private:
            Ui::SceneHierarchyPanel ui;
            QHeaderView*            header;
        };
    }    // namespace Editor
}    // namespace Razix