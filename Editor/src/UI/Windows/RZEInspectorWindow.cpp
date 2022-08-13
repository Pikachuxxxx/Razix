#include "RZEInspectorWindow.h"

#include "Razix/Scene/Components/RZComponents.h"

namespace Razix {
    namespace Editor {
        RZEInspectorWindow::RZEInspectorWindow(RZESceneHierarchyPanel* hierarchyPanel, QFrame* parent)
            : QFrame(parent)
        {
            ui.setupUi(this);

            ui.verticalLayout_2->setSpacing(5);
            ui.verticalLayout_2->setMargin(0);

            ui.UUIDLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);

            // Make the connections
            // Name change
            connect(ui.EntityName, SIGNAL(returnPressed()), this, SLOT(OnNameEdit()));
            // On entity selected
            connect(hierarchyPanel, &RZESceneHierarchyPanel::OnEntitySelected, this, &RZEInspectorWindow::OnEntitySelected);
        }

        RZEInspectorWindow::~RZEInspectorWindow()
        {
        }

        void RZEInspectorWindow::OnNameEdit()
        {
            // Update the entity name and repaint the Hierarchy panel to reflect the name
            auto& tagComponent = entity.GetComponent<TagComponent>();
            tagComponent.Tag   = ui.EntityName->text().toStdString();
            // TODO: Send the repaint event to RZESceneHierarchyPanel
        }

        void RZEInspectorWindow::OnEntitySelected(RZEntity entity)
        {
            this->entity = entity;
            ui.EntityName->setText(entity.GetComponent<TagComponent>().Tag.c_str());
        }

    }    // namespace Editor
}    // namespace Razix
