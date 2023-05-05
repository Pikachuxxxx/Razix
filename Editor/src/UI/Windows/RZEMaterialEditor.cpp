// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEMaterialEditor.h"

#include "Razix/Graphics/RHI/API/RZTexture.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include <QColorDialog>
#include <QFileDialog>

#include <glm/gtx/string_cast.hpp>

namespace Razix {
    namespace Editor {
        RZEMaterialEditor::RZEMaterialEditor(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            setObjectName("Material Editor");

            // Connections
            connect(ui.diffuseTexture, SIGNAL(pressed()), this, SLOT(on_DiffuseTextureSelect()));
            connect(ui.diffuseColor, SIGNAL(pressed()), this, SLOT(on_DiffuseColor()));
            connect(ui.specTexture, SIGNAL(pressed()), this, SLOT(on_SpecularTextureSelected()));
            connect(ui.metallicValue, SIGNAL(returnPressed()), this, SLOT(on_MetallicValueSet()));
            connect(ui.roughnessValue, SIGNAL(returnPressed()), this, SLOT(on_RoughnessValueSet()));
            connect(ui.emissiveIntensity, SIGNAL(returnPressed()), this, SLOT(on_EmissionIntensity()));
        }

        RZEMaterialEditor::~RZEMaterialEditor()
        {
        }

        void RZEMaterialEditor::OnSetEditingMaterial(Razix::Graphics::RZMaterial* material)
        {
            m_Material = material;

            // Set the Material Editor Properties
            ui.materialNameLbl->setText(material->getName().c_str());

            // Diffuse stuff
            auto& props    = material->getProperties();
            m_DiffuseColor = QColor(props.albedoColor.x, props.albedoColor.y, props.albedoColor.z);
            ui.diffuseColor->setStyleSheet("background-color: " + m_DiffuseColor.name());

            // Specular
            ui.specIntensity->setText(std::to_string(props.specularColor).c_str());

            // Emission
            ui.emissiveIntensity->setText(std::to_string(props.emissiveIntensity).c_str());

            // Metallic
            ui.metallicValue->setText(std::to_string(props.metallicColor).c_str());

            // Roughness
            ui.roughnessValue->setText(std::to_string(props.roughnessColor).c_str());
        }

        void RZEMaterialEditor::on_DiffuseTextureSelect()
        {
            auto    fileName = QFileDialog::getOpenFileName(this, "Select Diffuse Texture", "");
            QPixmap pixmap(fileName);
            QIcon   ButtonIcon(pixmap);
            ui.diffuseTexture->setIcon(ButtonIcon);
            ui.diffuseTexture->setIconSize(QSize(40, 40));

            if (!m_Material)
                return;

            auto& matTextures = m_Material->getTextures();
            //matTextures.albedo->Release();
            matTextures.albedo = Graphics::RZTexture2D::CreateFromFile(RZ_DEBUG_NAME_TAG_STR_F_ARG(fileName.toStdString()) fileName.toStdString(), fileName.toStdString());
            m_Material->setTextures(matTextures);
        }

        void RZEMaterialEditor::onDiffuseTextureUseCheckbox()
        {
        }

        void RZEMaterialEditor::on_DiffuseColor()
        {
            QColorDialog::ColorDialogOptions options;
            options.setFlag(QColorDialog::ShowAlphaChannel);
            QColor color   = QColorDialog::getColor(m_DiffuseColor, nullptr, "Albedo Color", options);
            m_DiffuseColor = color;

            ui.diffuseColor->setStyleSheet("background-color: " + color.name());

            std::cout << glm::to_string(glm::vec3(color.redF(), color.greenF(), color.blueF())) << std::endl;

            if (!m_Material)
                return;

            auto matProps        = m_Material->getProperties();
            matProps.albedoColor = glm::vec3(color.redF(), color.greenF(), color.blueF());
            matProps.opacity     = color.alphaF();
            m_Material->setProperties(matProps);
        }

        void RZEMaterialEditor::on_SpecularTextureSelected()
        {
            auto    fileName = QFileDialog::getOpenFileName(this, "Select Specular Texture", "");
            QPixmap pixmap(fileName);
            QIcon   ButtonIcon(pixmap);
            ui.specTexture->setIcon(ButtonIcon);
            ui.specTexture->setIconSize(QSize(40, 40));

            if (!m_Material)
                return;

            auto& matTextures = m_Material->getTextures();
            //matTextures.albedo->Release();
            matTextures.specular = Graphics::RZTexture2D::CreateFromFile(RZ_DEBUG_NAME_TAG_STR_F_ARG(fileName.toStdString()) fileName.toStdString(), fileName.toStdString());
            m_Material->setTextures(matTextures);
        }

        void RZEMaterialEditor::on_SpecularIntensity()
        {
            // Get the number from the LineEdit and set the specular float in the MaterialData struct
        }

        void RZEMaterialEditor::on_MetallicValueSet()
        {
            if (!m_Material)
                return;

            auto matProps          = m_Material->getProperties();
            matProps.metallicColor = ui.metallicValue->text().toFloat();
            m_Material->setProperties(matProps);
        }

        void RZEMaterialEditor::on_MetallicTextureSelected()
        {
        }

        void RZEMaterialEditor::on_RoughnessValueSet()
        {
            auto matProps           = m_Material->getProperties();
            matProps.roughnessColor = ui.roughnessValue->text().toFloat();
            m_Material->setProperties(matProps);
        }

        void RZEMaterialEditor::on_RoughnessTextureSelected()
        {
        }

        void RZEMaterialEditor::on_EmissionIntensity()
        {
            if (!m_Material)
                return;

            auto matProps              = m_Material->getProperties();
            matProps.emissiveIntensity = ui.emissiveIntensity->text().toFloat();
            m_Material->setProperties(matProps);
        }
    }    // namespace Editor
}    // namespace Razix