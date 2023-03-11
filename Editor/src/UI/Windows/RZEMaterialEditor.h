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
            // TODO: In Future use the reflection data to create the fields automatically, since we use a single Material this is perfectly acceptable
            // Diffuse
            void on_diffuse_texture_select();
            void on_diffuse_texture_use_checkbox();
            void on_diffuse_color();
            // Specular

        private:
            Ui::MaterialEditor           ui;
            Razix::Graphics::RZMaterial* m_Material;
        };
    }    // namespace Editor
}    // namespace Razix