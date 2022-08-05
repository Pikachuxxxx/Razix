#pragma once

#include <QFrame>
#include <QVBoxLayout>

#include "generated/ui_RZEInspectorWindow.h"

// All components will be added to this window and will be enabled on a need to basis; We can remove or add (only single isntance for now)
// This way code and design get's easier, but the Component widgets will have seperate UI and class files for ease of management;
// Will deriving from a common RZComponent and RZEComponentUI will benefit with plugin development and reduce code duplication? If so Implement it
namespace Razix {
    namespace Editor {
        class RZEInspectorWindow : public QFrame
        {
            Q_OBJECT

        public:
            RZEInspectorWindow(QFrame* parent = nullptr);
            ~RZEInspectorWindow();

            QVBoxLayout& getBoxLayout() { return *(ui.verticalLayout_2); }

        private:
            Ui::InspectorWindow ui;

        public slots:
            void OnNameEdit();
        };
    }    // namespace Editor
}    // namespace Razix
