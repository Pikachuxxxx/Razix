#include "RZEMeshRendererComponentUI.h"

namespace Razix {
    namespace Editor {
        RZEMeshRendererComponentUI::RZEMeshRendererComponentUI(QWidget *parent)
            : QWidget(parent)
        {
            ui.setupUi(this);
        }

        RZEMeshRendererComponentUI::~RZEMeshRendererComponentUI()
        {}

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
