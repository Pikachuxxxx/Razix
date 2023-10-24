// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEInspectorWindow.h"

#include "Razix/Graphics/RZMesh.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "UI/Widgets/RZECollapsingHeader.h"

#include "RZEMainWindow.h"

namespace Razix {
    namespace Editor {
        RZEInspectorWindow::RZEInspectorWindow(RZESceneHierarchyPanel* hierarchyPanel, QFrame* parent)
            : QFrame(parent)
        {
            ui.setupUi(this);

            setObjectName(this->windowTitle());

            ui.scrollLayout->setSpacing(5);
            ui.scrollLayout->setMargin(0);

            ui.UUIDLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);

            this->setContextMenuPolicy(Qt::CustomContextMenu);

            // Add the list of components that will be enabled and disables based on the entity
            // At index 0 and 1 we have the Tag and ID components in the box layout by the QDesigner
            // And Check boxes for Active Component and Static Mesh
            // Starting with 2 components already in (Tag + ID{Active+Static})
            // 2. Transform component
            m_TrasformComponentUI      = new Razix::Editor::RZETransformComponentUI;
            m_TrasformComponentSection = new Razix::Editor::RZECollapsingHeader(QString("Transform"), m_TrasformComponentUI, new QIcon(":/rzeditor/transform_icon.png"));
            connect(m_TrasformComponentSection, SIGNAL(utilButtonClicked()), this, SLOT(ShowComponentsUtilContextMenu()));

            // Since everything has a Transform Component
            this->getBoxLayout().insertWidget(RZ_FLAG_COMPONENT_TRANSFORM, m_TrasformComponentSection);

            initComponents();

            m_ComponentUtilMenu = new QMenu("Add Component", this);

            m_ComponentUtilMenu->addAction("AddComponent");

            m_ComponentUtilMenu->addSeparator();

            m_AddCameraComponent = new QAction("Camera Component", this);
            m_ComponentUtilMenu->addAction(m_AddCameraComponent);
            connect(m_AddCameraComponent, SIGNAL(triggered()), this, SLOT(AddCameraComponent()));

            m_AddLightComponent = new QAction("Light Component", this);
            m_ComponentUtilMenu->addAction(m_AddLightComponent);
            connect(m_AddLightComponent, SIGNAL(triggered()), this, SLOT(AddLightComponent()));

            m_AddLuaScriptComponent = new QAction("Lua Script Component", this);
            m_ComponentUtilMenu->addAction(m_AddLuaScriptComponent);
            connect(m_AddLuaScriptComponent, SIGNAL(triggered()), this, SLOT(AddLuaScriptComponent()));

            m_AddMeshRendererComponent = new QAction("Mesh Renderer Component", this);
            m_ComponentUtilMenu->addAction(m_AddMeshRendererComponent);
            connect(m_AddMeshRendererComponent, SIGNAL(triggered()), this, SLOT(AddMeshRendererComponent()));

            m_AddSpriteRendererComponent = new QAction("Sprite Renderer Component", this);
            m_ComponentUtilMenu->addAction(m_AddSpriteRendererComponent);
            connect(m_AddSpriteRendererComponent, SIGNAL(triggered()), this, SLOT(AddSpriteRendererComponent()));

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
                connect(m_CameraComponentSection, SIGNAL(utilButtonClicked()), this, SLOT(ShowComponentsUtilContextMenu()));
                this->getBoxLayout().insertWidget(idx, m_CameraComponentSection);
                m_CameraComponentSection->setVisible(true);
                // Set the Editing Entity
                m_CameraComponentUI->setEditingEntity(entity);
                idx++;
            }
            if (entity.HasComponent<LightComponent>()) {
                m_ComponentsMask |= RZ_FLAG_COMPONENT_LIGHT;
                connect(m_LightComponentSection, SIGNAL(utilButtonClicked()), this, SLOT(ShowComponentsUtilContextMenu()));
                this->getBoxLayout().insertWidget(idx, m_LightComponentSection);
                m_LightComponentSection->setVisible(true);
                // Set the editing entity
                m_LightComponentUI->setEditingEntity(entity);
                idx++;
            }
            if (entity.HasComponent<LuaScriptComponent>()) {
                m_ComponentsMask |= RZ_FLAG_COMPONENT_LUA_SCRIPT;
                connect(m_LuaScriptComponentSection, SIGNAL(utilButtonClicked()), this, SLOT(ShowComponentsUtilContextMenu()));
                this->getBoxLayout().insertWidget(idx, m_LuaScriptComponentSection);
                m_LuaScriptComponentSection->setVisible(true);
                // Set the editing entity
                m_LuaScriptComponentUI->setEditingEntity(entity);
                idx++;
            }
            if (entity.HasComponent<MeshRendererComponent>()) {
                m_ComponentsMask |= RZ_FLAG_COMPONENT_MESH_RENDERER;
                connect(m_MeshRendererComponentSection, SIGNAL(utilButtonClicked()), this, SLOT(ShowComponentsUtilContextMenu()));
                this->getBoxLayout().insertWidget(idx, m_MeshRendererComponentSection);
                m_MeshRendererComponentSection->setVisible(true);
                // Connect the entity and the MRC UI
                m_MeshRendererComponentUI->setEditingEntity(entity);
                auto material = entity.GetComponent<MeshRendererComponent>().Mesh->getMaterial();
                if (material)
                    emit OnMeshMaterialSelected(material);
                idx++;
            }
            if (entity.HasComponent<SpriteRendererComponent>()) {
                m_ComponentsMask |= RZ_FLAG_COMPONENT_SPRITE_RENDERER;
                connect(m_SpriteRendererComponentSection, SIGNAL(utilButtonClicked()), this, SLOT(ShowComponentsUtilContextMenu()));
                this->getBoxLayout().insertWidget(idx, m_SpriteRendererComponentSection);
                m_SpriteRendererComponentSection->setVisible(true);
                idx++;
            }

            // Update the transform component
            m_TrasformComponentUI->setEditingEntity(entity);
        }

        void RZEInspectorWindow::ShowComponentsUtilContextMenu()
        {
            m_ComponentUtilMenu->exec(QCursor::pos());
        }

        void RZEInspectorWindow::AddCameraComponent()
        {
            m_InspectingEntity.AddComponent<CameraComponent>();
            OnEntitySelected(m_InspectingEntity);
        }

        void RZEInspectorWindow::AddLightComponent()
        {
            m_InspectingEntity.AddComponent<LightComponent>();
            OnEntitySelected(m_InspectingEntity);
        }

        void RZEInspectorWindow::AddLuaScriptComponent()
        {
            m_InspectingEntity.AddComponent<LuaScriptComponent>();
            OnEntitySelected(m_InspectingEntity);
        }

        void RZEInspectorWindow::AddMeshRendererComponent()
        {
            m_InspectingEntity.AddComponent<MeshRendererComponent>(Graphics::MeshPrimitive::Sphere);
            OnEntitySelected(m_InspectingEntity);
        }

        void RZEInspectorWindow::AddSpriteRendererComponent()
        {
            m_InspectingEntity.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
            OnEntitySelected(m_InspectingEntity);
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
