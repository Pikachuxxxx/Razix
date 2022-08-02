#include "RZEInspectorWindow.h"

namespace Razix {
    namespace Editor {
        RZEInspectorWindow::RZEInspectorWindow(QFrame *parent)
            : QFrame(parent)
        {
            ui.setupUi(this);

            ui.verticalLayout_2->setSpacing(0);
        }

        RZEInspectorWindow::~RZEInspectorWindow() 
        {
        }

    }    // namespace Editor
}    // namespace Razix
