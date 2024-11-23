#pragma once

#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

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

            void setEditingEntity(RZEntity entity);

        public slots:
            void on_mesh_type_selected(int meshType);

            void OnMeshLocationEdited();
            void OnMaterialLocationEdited();

            void OnEnableBoundingBoxPressed();
            void OnReceiveShadowsPressed();

            void OnBrowseMesh();
            void OnBrowseMaterial();
            void OnAddNewMaterial();

        private:
            Ui::MeshRendererComponentUI ui;
            Razix::RZEntity             m_Entity;
        };
    }    // namespace Editor
}    // namespace Razix
