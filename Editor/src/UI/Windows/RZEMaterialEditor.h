#pragma once

#include <QWidget>

#include "generated/ui_RZEMaterialEditor.h"

namespace Razix {

    namespace Graphics {
        class RZMaterial;
    }

    namespace Editor {

        /**
         * The material Editor edits the material set, now is the material set?
         * 
         */
        class RZEMaterialEditor : public QWidget
        {
            Q_OBJECT

        public:
            RZEMaterialEditor(QWidget* parent = nullptr);
            ~RZEMaterialEditor();

            void setEditingMaterial(Razix::Graphics::RZMaterial* material);

        public slots:
            void on_diffuse_texture_select();
            void on_diffuse_texture_use_checkbox();
            void on_diffuse_color();

        private:
            Ui::MaterialEditor           ui;
            Razix::Graphics::RZMaterial* m_Material;
        };
    }    // namespace Editor
}    // namespace Razix