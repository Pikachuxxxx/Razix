#pragma once

#include "generated/ui_RZEMeshRendererComponentUI.h"

#include <QWidget>

namespace Razix {
    namespace Editor {
        class RZEMeshRendererComponentUI : public QWidget
        {
            Q_OBJECT

        public:
            RZEMeshRendererComponentUI(QWidget *parent = nullptr);
            ~RZEMeshRendererComponentUI();

        public slots:
            void on_mesh_type_selected(int meshType);

            void OnMeshLocationEdited();
            void OnMaterialLocationEdited();

            void OnBrowseMesh();
            void OnBrowseMaterial();
        private:
            Ui::MeshRendererComponentUI ui; 
        };
    }    // namespace Editor
}    // namespace Razix
