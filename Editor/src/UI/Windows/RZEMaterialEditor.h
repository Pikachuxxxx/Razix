#pragma once

#include <QWidget>

#include "generated/ui_RZEMaterialEditor.h"

namespace Razix {

    namespace Gfx {
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
            void OnSetEditingMaterial(Razix::Gfx::RZMaterial* material);
            void on_NameChanged();
            // TODO: In Future use the reflection data to create the fields automatically, since we use a single Material this is perfectly acceptable
            // Diffuse
            void on_DiffuseTextureSelect();
            void onDiffuseTextureUseCheckbox();
            void on_DiffuseColor();
            // Specular
            void on_SpecularTextureSelected();
            void on_SpecularIntensity();
            // Normal
            void on_NormaTextureSelected();
            // Metallic
            void on_MetallicValueSet();
            void on_MetallicTextureSelected();
            // Roughness
            void on_RoughnessValueSet();
            void on_RoughnessTextureSelected();
            // Emission
            /* Used for HDR Bloom intensity */
            void on_EmissionIntensity();
            // AO
            void on_AOValueSet();
            void on_AOTextureSelected();
            // UV scale
            void OnUVScaleXChanged();
            void OnUVScaleYChanged();

        private:
            Ui::MaterialEditor           ui;
            Razix::Gfx::RZMaterial* m_Material = nullptr;
            QColor                       m_DiffuseColor;
        };
    }    // namespace Editor
}    // namespace Razix