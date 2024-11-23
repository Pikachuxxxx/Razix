// clang-format off
#include "rzepch.h"
// clang-format on

#include "RZEMeshRendererComponentUI.h"

#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZMeshFactory.h"
#include "Razix/Scene/Components/MeshRendererComponent.h"

#include <QInputDialog>

namespace Razix {
    namespace Editor {
        RZEMeshRendererComponentUI::RZEMeshRendererComponentUI(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_mesh_type_selected(int)));
        }

        RZEMeshRendererComponentUI::~RZEMeshRendererComponentUI()
        {
        }

        void RZEMeshRendererComponentUI::setEditingEntity(RZEntity entity)
        {
            m_Entity = entity;
        }

        void RZEMeshRendererComponentUI::on_mesh_type_selected(int meshType)
        {
            auto& mrc = m_Entity.GetComponent<MeshRendererComponent>();
            //Graphics::RZMesh* oldMesh = mrc.Mesh;
            if (mrc.Mesh)
                mrc.Mesh = Razix::Gfx::MeshFactory::CreatePrimitive((Razix::Gfx::MeshPrimitive) meshType);
            //oldMesh->Destroy();
            // TODO: Update the material in the Material Editor (emit a signal)
        }

        void RZEMeshRendererComponentUI::OnMeshLocationEdited()
        {
        }

        void RZEMeshRendererComponentUI::OnMaterialLocationEdited()
        {
        }

        void RZEMeshRendererComponentUI::OnEnableBoundingBoxPressed()
        {
            auto& mrc = m_Entity.GetComponent<MeshRendererComponent>();
            if (m_Entity.HasComponent<MeshRendererComponent>())
                mrc.enableBoundingBoxes = ui.showBoundingBox->isChecked();
        }

        void RZEMeshRendererComponentUI::OnReceiveShadowsPressed()
        {
            auto& mrc = m_Entity.GetComponent<MeshRendererComponent>();
            if (m_Entity.HasComponent<MeshRendererComponent>())
                mrc.receiveShadows = ui.receiveShadows->isChecked();
        }

        void RZEMeshRendererComponentUI::OnBrowseMesh()
        {
        }

        void RZEMeshRendererComponentUI::OnBrowseMaterial()
        {
        }

        void RZEMeshRendererComponentUI::OnAddNewMaterial()
        {
            QString text = QInputDialog::getText(this, "Enter New Material Name", "Name");
        }

    }    // namespace Editor
}    // namespace Razix
