// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEFrameGraphEditor.h"

namespace Razix {
    namespace Editor {

        RZEFrameGraphEditor::RZEFrameGraphEditor(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);


            m_NodeGraphWidget = new NodeGraphWidget;

            ui.body_layout->addWidget(m_NodeGraphWidget);
        }

    }    // namespace Editor
}    // namespace Razix