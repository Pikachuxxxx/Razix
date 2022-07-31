#include "rzxpch.h"
#include "RZEMainWindow.h"

namespace Razix {
    namespace Editor {

        RZEMainWindow::RZEMainWindow(QWidget *parent /*= Q_NULLPTR*/, Qt::WindowFlags flags /*= {}*/)
            // Create the QApplication here and run it on a separate thread + pass the necessary native window handles to the Razix Application
            : QMainWindow(parent, flags)
        {
            // Link the UI file with this class
            ui.setupUi(this);

            //ui.toolWindowManager->setToolWindowCreateCallback([this](const QString &objectName) -> QWidget * {
            //    std::cout << "Window added : " << objectName.toStdString() << std::endl;
            //    return 
            //});
        }

    }    // namespace Editor
}    // namespace Razix
