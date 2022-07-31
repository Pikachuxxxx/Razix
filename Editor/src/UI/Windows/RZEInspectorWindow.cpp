#include "RZEInspectorWindow.h"

namespace Razix {
    namespace Editor {
        RZEInspectorWindow::RZEInspectorWindow(QFrame *parent)
            : QFrame(parent)
        {
            ui.setupUi(this);
        }

        RZEInspectorWindow::~RZEInspectorWindow()
        {}
    }    // namespace Editor
}    // namespace Razix
