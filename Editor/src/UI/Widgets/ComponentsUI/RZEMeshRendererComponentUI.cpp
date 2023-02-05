#include "RZEMeshRendererComponentUI.h"

namespace Razix {
    namespace Editor {
        RZEMeshRendererComponentUI::RZEMeshRendererComponentUI(QWidget *parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_mesh_type_selected(int)));
        }

        RZEMeshRendererComponentUI::~RZEMeshRendererComponentUI()
        {
        }

        void RZEMeshRendererComponentUI::on_mesh_type_selected(int meshType)
        {
            printf("Mesh Type Selected : %d", meshType);
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
