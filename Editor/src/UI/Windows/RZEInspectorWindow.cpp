#include "RZEInspectorWindow.h"

namespace Razix {
    namespace Editor {
        RZEInspectorWindow::RZEInspectorWindow(QFrame *parent)
            : QFrame(parent)
        {
            ui.setupUi(this);

            // First add a QVBoxLayout
            auto layout = new QVBoxLayout(this);
        }

        RZEInspectorWindow::~RZEInspectorWindow() 
        {}

    }    // namespace Editor
}    // namespace Razix
