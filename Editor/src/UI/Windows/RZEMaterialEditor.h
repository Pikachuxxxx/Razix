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

        public slots:
            void OnSetEditingMaterial(Razix::Graphics::RZMaterial* material);
            // TODO: In Future use the reflection data to create the fields automatically, since we use a single Material this is perfectly acceptable
            // Diffuse
            void on_DiffuseTextureSelect();
            void onDiffuseTextureUseCheckbox();
            void on_DiffuseColor();
            // Specular
            void on_SpecularTextureSelected();
            void on_SpecularIntensity();
            // Emission
            /* Used for HDR Bloom intensity */
            void on_EmissionIntensity(); 

        private:
            Ui::MaterialEditor           ui;
            Razix::Graphics::RZMaterial* m_Material;
            QColor                       m_DiffuseColor;
        };
    }    // namespace Editor
}    // namespace Razix