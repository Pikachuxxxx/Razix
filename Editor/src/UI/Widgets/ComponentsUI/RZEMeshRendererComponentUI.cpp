// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZEMeshRendererComponentUI.h"

#include "Razix/Scene/Components/MeshRendererComponent.h"
#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

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
            //mrc.Mesh->Destroy();
            mrc.Mesh = Razix::Graphics::MeshFactory::CreatePrimitive((Razix::Graphics::MeshPrimitive) meshType);
        }

        void RZEMeshRendererComponentUI::OnMeshLocationEdited()
        {
        }

        void RZEMeshRendererComponentUI::OnMaterialLocationEdited()
        {
        }

        void RZEMeshRendererComponentUI::OnBrowseMesh()
        {
        }

        void RZEMeshRendererComponentUI::OnBrowseMaterial()
        {
        }

    }    // namespace Editor
}    // namespace Razix
