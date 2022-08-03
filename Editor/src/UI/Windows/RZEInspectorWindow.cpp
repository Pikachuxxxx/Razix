#include "RZEInspectorWindow.h"

namespace Razix {
    namespace Editor {
        RZEInspectorWindow::RZEInspectorWindow(QFrame *parent)
            : QFrame(parent)
        {
            ui.setupUi(this);

            ui.verticalLayout_2->setSpacing(5);
            ui.verticalLayout_2->setMargin(0);

            ui.UUIDLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
        }

        RZEInspectorWindow::~RZEInspectorWindow() 
        {
        }

    }    // namespace Editor
}    // namespace Razix
