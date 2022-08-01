#pragma once

#include <QFrame>

#include "generated/ui_RZEInspectorWindow.h"
#include "generated/ui_RZETransformComponentUI.h"

// All components will be added to this window and will be enabled on a need to basis; We can remove or add (only single isntance for now)
// This way code and design get's easier, but the Component widgets will have seperate UI and class files for easy of management;
namespace Razix {
    namespace Editor {
        class RZEInspectorWindow : public QFrame
        {
            Q_OBJECT

        public:
            RZEInspectorWindow(QFrame *parent = nullptr);
            ~RZEInspectorWindow();

        private:
            Ui::InspectorWindow ui;
            Ui::TransformComponent ui_TransformComponent;

        };
    }    // namespace Editor
}    // namespace Razix
