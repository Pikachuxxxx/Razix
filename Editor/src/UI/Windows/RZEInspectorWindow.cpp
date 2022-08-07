#include "RZEInspectorWindow.h"

#include <iostream>
namespace Razix {
    namespace Editor {
        RZEInspectorWindow::RZEInspectorWindow(QFrame *parent)
            : QFrame(parent)
        {
            ui.setupUi(this);

            ui.verticalLayout_2->setSpacing(5);
            ui.verticalLayout_2->setMargin(0);

            ui.UUIDLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
            
            // Make the connections
            connect(ui.EntityName, SIGNAL(returnPressed()), this, SLOT(OnNameEdit()));
        }

        RZEInspectorWindow::~RZEInspectorWindow()
        {
        }

        void RZEInspectorWindow::OnNameEdit()
        {
            std::cout << "Entity name edited to : " << ui.EntityName->text().toStdString() << std::endl;
        }

    }    // namespace Editor
}    // namespace Razix
