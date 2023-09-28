#pragma once

#include <QtNodeGraph.h>

#include "generated/ui_RZEFrameGraphEditor.h"

namespace Razix {
    namespace Editor {
        class RZEFrameGraphEditor : public QWidget
        {
            Q_OBJECT

        public:
            RZEFrameGraphEditor(QWidget* parent = nullptr);
            ~RZEFrameGraphEditor() {}

        private:
            NodeGraphWidget*     m_NodeGraphWidget;
            Ui::FrameGraphEditor ui;
        };
    }    // namespace Editor
}    // namespace Razix
