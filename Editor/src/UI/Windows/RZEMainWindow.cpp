#include "RZEMainWindow.h"

namespace Razix {
    namespace Editor {

        RZEMainWindow::RZEMainWindow(QWidget *parent /*= Q_NULLPTR*/, Qt::WindowFlags flags /*= {}*/)
            : QMainWindow(parent, flags)
        {
            ui.setupUi(this);
        }

    }    // namespace Editor
}    // namespace Razix
