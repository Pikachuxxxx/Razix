#pragma once

#include <QFrame>

#include "generated/ui_RZEInspectorWindow.h"

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
        };
    }    // namespace Editor
}    // namespace Razix
