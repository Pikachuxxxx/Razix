#include "RZEMainWindow.h"

namespace Razix {
    namespace Editor {

        RZEMainWindow::RZEMainWindow(QWidget *parent /*= Q_NULLPTR*/, Qt::WindowFlags flags /*= {}*/)
            // Create the QApplication here and run it on a separate thread + pass the necessary native window handles to the Razix Application
            : QMainWindow(parent, flags)
        {
            ui.setupUi(this);
        }

    }    // namespace Editor
}    // namespace Razix
