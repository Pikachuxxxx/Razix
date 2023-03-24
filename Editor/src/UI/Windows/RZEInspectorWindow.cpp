// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEInspectorWindow.h"

#include "Razix/Graphics/RZMesh.h"

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
            // Starting with 2 components already in (Tag + ID{Active+Static})
            // 2. Transform component
            m_TrasformComponentUI      = new Razix::Editor::RZETransformComponentUI;
            m_TrasformComponentSection = new Razix::Editor::RZECollapsingHeader(QString("Transform"), m_TrasformComponentUI, new QIcon(":/rzeditor/transform_icon.png"));
            // Since everything has a Transform Component
            this->getBoxLayout().insertWidget(RZ_FLAG_COMPONENT_TRANSFORM, m_TrasformComponentSection);

            initComponents();

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
            // Set the editing entity
            m_TrasformComponentUI->setEditingEntity(entity);

            // Update the name label
            ui.EntityName->setText(entity.GetComponent<TagComponent>().Tag.c_str());

            // Update the UUID
            ui.UUIDLbl->setText(entity.GetComponent<IDComponent>().UUID.prettyString().c_str());

            destroyComponents();

            // Set the components to enable for the selected entity in the Inspector Panel
            // By default every component has a ID, Tag and Transform component
            m_ComponentsMask |= RZ_FLAG_COMPONENT_ACTIVE | RZ_FLAG_COMPONENT_ID | RZ_FLAG_COMPONENT_TAG | RZ_FLAG_COMPONENT_TRANSFORM;
            uint32_t idx = this->getBoxLayout().count() - 1;
            if (entity.HasComponent<CameraComponent>()) {
                m_ComponentsMask |= RZ_FLAG_COMPONENT_CAMERA;
                this->getBoxLayout().insertWidget(idx, m_CameraComponentSection);
                m_CameraComponentSection->setVisible(true);
                idx++;
            }
            if (entity.HasComponent<LightComponent>()) {
                m_ComponentsMask |= RZ_FLAG_COMPONENT_LIGHT;
                this->getBoxLayout().insertWidget(idx, m_LightComponentSection);
                m_LightComponentSection->setVisible(true);
                // Set the editing entity
                m_LightComponentUI->setEditingEntity(entity);
                idx++;
            }
            if (entity.HasComponent<LuaScriptComponent>()) {
                m_ComponentsMask |= RZ_FLAG_COMPONENT_LUA_SCRIPT;
                this->getBoxLayout().insertWidget(idx, m_LuaScriptComponentSection);
                m_LuaScriptComponentSection->setVisible(true);
                idx++;
            }
            if (entity.HasComponent<MeshRendererComponent>()) {
                m_ComponentsMask |= RZ_FLAG_COMPONENT_MESH_RENDERER;
                this->getBoxLayout().insertWidget(idx, m_MeshRendererComponentSection);
                m_MeshRendererComponentSection->setVisible(true);
                // Connect the entity and the MRC UI
                m_MeshRendererComponentUI->setEditingEntity(entity);
                emit OnMeshMaterialSelected(entity.GetComponent<MeshRendererComponent>().Mesh->getMaterial());
                idx++;
            }
            if (entity.HasComponent<SpriteRendererComponent>()) {
                m_ComponentsMask |= RZ_FLAG_COMPONENT_SPRITE_RENDERER;
                this->getBoxLayout().insertWidget(idx, m_SpriteRendererComponentSection);
                m_SpriteRendererComponentSection->setVisible(true);
                idx++;
            }

            // Update the transform component
            m_TrasformComponentUI->setEditingEntity(entity);
        }

        void RZEInspectorWindow::initComponents()
        {
            // 3. Add the camera component
            m_CameraComponentUI      = new RZECameraComponentUI;
            m_CameraComponentSection = new Razix::Editor::RZECollapsingHeader(QString("Camera"), m_CameraComponentUI, new QIcon(":/rzeditor/camera_icon.png"));

            // 4. Add the Light Component
            m_LightComponentUI      = new RZELightComponentUI;
            m_LightComponentSection = new Razix::Editor::RZECollapsingHeader(QString("Light"), m_LightComponentUI, new QIcon(":/rzeditor/LightIcon.png"));

            // 5. Add the Lua Script Component
            m_LuaScriptComponentUI      = new RZELuaScriptComponentUI;
            m_LuaScriptComponentSection = new Razix::Editor::RZECollapsingHeader(QString("Lua Script"), m_LuaScriptComponentUI, new QIcon(":/rzeditor/RazixScriptFile.png"));

            // 6. Add the Mesh Renderer Component
            m_MeshRendererComponentUI      = new RZEMeshRendererComponentUI;
            m_MeshRendererComponentSection = new Razix::Editor::RZECollapsingHeader(QString("Mesh"), m_MeshRendererComponentUI, new QIcon(":/rzeditor/mesh.png"));

            // 7. Add the Sprite Renderer Component
            m_SpriteRendererComponentUI      = new RZESpriteRendererComponentUI;
            m_SpriteRendererComponentSection = new Razix::Editor::RZECollapsingHeader(QString("Sprite"), m_SpriteRendererComponentUI, new QIcon(":/rzeditor/sprite_can.png"));
        }

        void RZEInspectorWindow::destroyComponents()
        {
            // Reset the component flags and components in the Inspector Window
            m_ComponentsMask = 0;
            this->getBoxLayout().removeWidget(m_CameraComponentSection);
            m_CameraComponentSection->setVisible(false);
            this->getBoxLayout().removeWidget(m_LightComponentSection);
            m_LightComponentSection->setVisible(false);
            this->getBoxLayout().removeWidget(m_LuaScriptComponentSection);
            m_LuaScriptComponentSection->setVisible(false);
            this->getBoxLayout().removeWidget(m_MeshRendererComponentSection);
            m_MeshRendererComponentSection->setVisible(false);
            this->getBoxLayout().removeWidget(m_SpriteRendererComponentSection);
            m_SpriteRendererComponentSection->setVisible(false);
        }

    }    // namespace Editor
}    // namespace Razix
