#include "RZEInspectorWindow.h"

namespace Razix {
    namespace Editor {
        RZEInspectorWindow::RZEInspectorWindow(QFrame *parent)
            : QFrame(parent)
        {
            ui.setupUi(this);

            QWidget* transformComponentWidget = new QWidget(this);
            ui_TransformComponent.setupUi(transformComponentWidget);

            auto layout = new QVBoxLayout();
            layout->addWidget(transformComponentWidget);
            this->setLayout(layout);
        }

        RZEInspectorWindow::~RZEInspectorWindow()
        {}

    }    // namespace Editor
}    // namespace Razix
