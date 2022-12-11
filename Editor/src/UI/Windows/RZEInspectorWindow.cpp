#include "RZEInspectorWindow.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "UI/Widgets/RZECollapsingHeader.h"

namespace Razix {
    namespace Editor {
        RZEInspectorWindow::RZEInspectorWindow(RZESceneHierarchyPanel* hierarchyPanel, QFrame* parent)
            : QFrame(parent)
        {
            ui.setupUi(this);

            ui.scrollLayout->setSpacing(5);
            ui.scrollLayout->setMargin(0);

            ui.UUIDLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);

            // Add the list of components that will be enabled and disables based on the entity
            // At index 0 and 1 we have the Tag and ID components in the box layout by the QDesigner
            // And Check boxes for Active Component and Static Mesh
            uint32_t idx = 2;    // Starting with 2 components already in (Tag + ID{Active+Static})
            // 1. Transform component
            m_TrasformComponentUI = new Razix::Editor::RZETransformComponentUI;
            this->getBoxLayout().insertWidget(idx, (new Razix::Editor::RZECollapsingHeader(QString("Transform"), m_TrasformComponentUI, new QIcon(":/rzeditor/transform_icon.png"))));

            // 2. Add the camera component
            m_CameraComponentUI = new RZECameraComponentUI;
            idx++;
            this->getBoxLayout().insertWidget(idx, (new Razix::Editor::RZECollapsingHeader(QString("Camera"), m_CameraComponentUI, new QIcon(":/rzeditor/camera_icon.png"))));

            // 3. Add the Mesh Renderer Component
            m_MeshRendererComponentUI = new RZEMeshRendererComponentUI;
            idx++;
            this->getBoxLayout().insertWidget(idx, (new Razix::Editor::RZECollapsingHeader(QString("MeshRenderer"), m_MeshRendererComponentUI, new QIcon(":/rzeditor/mesh.png"))));

            // 4. Add the Lua Script Component
            m_LuaScriptComponentUI = new RZELuaScriptComponentUI;
            idx++;
            this->getBoxLayout().insertWidget(idx, (new Razix::Editor::RZECollapsingHeader(QString("Lua Script"), m_LuaScriptComponentUI, new QIcon(":/rzeditor/RazixScriptFile.png"))));

            // 5. Add the Light Component
            m_LightComponentUI = new RZELightComponentUI;
            idx++;
            this->getBoxLayout().insertWidget(idx, (new Razix::Editor::RZECollapsingHeader(QString("Light Component"), m_LightComponentUI, new QIcon(":/rzeditor/LightIcon.png"))));

            // 6. Add the Sprite Renderer Component
            m_SpriteRendererComponentUI = new RZESpriteRendererComponentUI;
            idx++;
            this->getBoxLayout().insertWidget(idx, (new Razix::Editor::RZECollapsingHeader(QString("Sprite Renderer"), m_SpriteRendererComponentUI, new QIcon(":/rzeditor/sprite_can.png"))));

            // TODO: Add "Add Component" Button

            // connections
            // Name change
            connect(ui.EntityName, SIGNAL(returnPressed()), this, SLOT(OnNameEdit()));
            // On Entity selected
            connect(hierarchyPanel, &RZESceneHierarchyPanel::OnEntitySelected, this, &RZEInspectorWindow::OnEntitySelected);
            // repaint the hierarchy panel for name changes
            connect(this, SIGNAL(InspectorPropertyChanged()), hierarchyPanel, SLOT(UpdatePanel()));
        }

        RZEInspectorWindow::~RZEInspectorWindow()
        {
        }

        void RZEInspectorWindow::OnNameEdit()
        {
            // Update the entity name and repaint the Hierarchy panel to reflect the name
            auto& tagComponent = m_InspectingEntity.GetComponent<TagComponent>();
            tagComponent.Tag   = ui.EntityName->text().toStdString();
            // Send the repaint event to RZESceneHierarchyPanel
            emit InspectorPropertyChanged();
        }

        void RZEInspectorWindow::OnEntitySelected(RZEntity entity)
        {
            m_InspectingEntity = entity;

            // Update the name label
            ui.EntityName->setText(entity.GetComponent<TagComponent>().Tag.c_str());

            // Update the UUID
            ui.UUIDLbl->setText(entity.GetComponent<IDComponent>().UUID.prettyString().c_str());

            // Set the components to enable for the selected entity in the Inspector Panel
            // By default every component has a ID, Tag and Transform component
            m_ComponentsMask |= RZ_FLAG_COMPONENT_ID | RZ_FLAG_COMPONENT_TAG | RZ_FLAG_COMPONENT_TRANSFORM;
            if (entity.HasComponent<CameraComponent>())
                m_ComponentsMask |= RZ_FLAG_COMPONENT_CAMERA;

            // Update the transform component
            m_TrasformComponentUI->setEditingEntity(entity);
        }

    }    // namespace Editor
}    // namespace Razix
