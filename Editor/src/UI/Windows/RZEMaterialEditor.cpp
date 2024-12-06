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
            connect(ui.materialName, SIGNAL(returnPressed()), this, SLOT(on_NameChanged()));

            connect(ui.diffuseTexture, SIGNAL(pressed()), this, SLOT(on_DiffuseTextureSelect()));
            connect(ui.diffuseColor, SIGNAL(pressed()), this, SLOT(on_DiffuseColor()));
            connect(ui.specTexture, SIGNAL(pressed()), this, SLOT(on_SpecularTextureSelected()));
            connect(ui.normalTexture, SIGNAL(pressed()), this, SLOT(on_NormaTextureSelected()));
            connect(ui.metallicValue, SIGNAL(returnPressed()), this, SLOT(on_MetallicValueSet()));
            connect(ui.metallicTexture, SIGNAL(pressed()), this, SLOT(on_MetallicTextureSelected()));
            connect(ui.roughnessValue, SIGNAL(returnPressed()), this, SLOT(on_RoughnessValueSet()));
            connect(ui.roughnessTexture, SIGNAL(pressed()), this, SLOT(on_RoughnessTextureSelected()));
            connect(ui.emissiveIntensity, SIGNAL(returnPressed()), this, SLOT(on_EmissionIntensity()));
            connect(ui.aoIntensity, SIGNAL(returnPressed()), this, SLOT(on_AOValueSet()));
            connect(ui.aoTexture, SIGNAL(pressed()), this, SLOT(on_AOTextureSelected()));

            ui.UVScaleX->setValidator(new QIntValidator(1, 10000, this));
            ui.UVScaleY->setValidator(new QIntValidator(1, 10000, this));

            connect(ui.UVScaleX, SIGNAL(textChanged(const QString&)), this, SLOT(OnUVScaleXChanged()));
            connect(ui.UVScaleY, SIGNAL(textChanged(const QString&)), this, SLOT(OnUVScaleYChanged()));
        }

        RZEMaterialEditor::~RZEMaterialEditor()
        {
        }

        void RZEMaterialEditor::OnSetEditingMaterial(Razix::Graphics::RZMaterial* material)
        {
            m_Material = material;

            // Set the Material Editor Properties
            ui.materialName->setText(material->getName().c_str());

            const std::string defaultTextureQSS = "image:url(:/rzeditor/select_texture_placeholder.png);";

            QPixmap pixmap(defaultTextureQSS.c_str());
            QIcon   icon(pixmap);
            ui.diffuseTexture->setIconSize(QSize(50, 50));
            ui.diffuseTexture->setIcon(icon);
            ui.normalTexture->setIconSize(QSize(50, 50));
            ui.normalTexture->setIcon(icon);
            ui.metalRoughnessAOMap->setIconSize(QSize(50, 50));
            ui.metalRoughnessAOMap->setIcon(icon);
            ui.metallicTexture->setIconSize(QSize(50, 50));
            ui.metallicTexture->setIcon(icon);
            ui.roughnessTexture->setIconSize(QSize(50, 50));
            ui.roughnessTexture->setIcon(icon);
            ui.aoTexture->setIconSize(QSize(50, 50));
            ui.aoTexture->setIcon(icon);

            // Diffuse stuff
            auto& props    = material->getProperties();
            auto& textures = material->getTexturePaths();
            m_DiffuseColor = QColor(props.albedoColor.x, props.albedoColor.y, props.albedoColor.z);
            QPalette pal   = ui.diffuseColor->palette();
            pal.setColor(QPalette::Button, m_DiffuseColor);
            ui.diffuseColor->setAutoFillBackground(true);
            ui.diffuseColor->setPalette(pal);
            if (props.isUsingAlbedoMap) {
                QPixmap pixmap(textures.albedo);
                QIcon   icon(textures.albedo);
                ui.diffuseTexture->setIconSize(QSize(50, 50));
                ui.diffuseTexture->setIcon(icon);
            }

            if (props.isUsingNormalMap) {
                QPixmap pixmap(textures.normal);
                QIcon   icon(pixmap);
                ui.normalTexture->setIconSize(QSize(50, 50));
                ui.normalTexture->setIcon(icon);
            }

            if (props.isUsingMetallicMap) {
                if (props.workflow == 0) {
                    QPixmap pixmap(textures.metallicRoughnessAO);
                    QIcon   icon(pixmap);
                    ui.metalRoughnessAOMap->setIconSize(QSize(50, 50));
                    ui.metalRoughnessAOMap->setIcon(icon);
                } else {
                    QPixmap pixmap(textures.metallic);
                    QIcon   icon(pixmap);
                    ui.metallicTexture->setIconSize(QSize(50, 50));
                    ui.metallicTexture->setIcon(icon);
                }
            }

            if (props.isUsingRoughnessMap) {
                QPixmap pixmap(textures.roughness);
                QIcon   icon(pixmap);
                ui.roughnessTexture->setIconSize(QSize(50, 50));
                ui.roughnessTexture->setIcon(icon);
            }

            if (props.isUsingAOMap) {
                QPixmap pixmap(textures.ao);
                QIcon   icon(pixmap);
                ui.aoTexture->setIconSize(QSize(50, 50));
                ui.aoTexture->setIcon(icon);
            }

            // Specular
            ui.specIntensity->setText(std::to_string(props.specularColor).c_str());

            // Emission
            ui.emissiveIntensity->setText(std::to_string(props.emissiveIntensity).c_str());

            // Metallic
            ui.metallicValue->setText(std::to_string(props.metallicColor).c_str());

            // Roughness
            ui.roughnessValue->setText(std::to_string(props.roughnessColor).c_str());
        }

        void RZEMaterialEditor::on_NameChanged()
        {
            if (!m_Material || ui.materialName->text().isEmpty())
                return;

            m_Material->setName(ui.materialName->text().toStdString());
        }

        //---------------------------------------------------------------------------
        void RZEMaterialEditor::on_DiffuseTextureSelect()
        {
            auto    fileName = QFileDialog::getOpenFileName(this, "Select Diffuse Texture", "");
            QPixmap pixmap(fileName);
            QIcon   ButtonIcon(pixmap);
            ui.diffuseTexture->setIcon(ButtonIcon);
            ui.diffuseTexture->setIconSize(QSize(40, 40));

            if (!m_Material && fileName.isEmpty())
                return;

            auto matTexturePaths = m_Material->getTexturePaths();
            memcpy(matTexturePaths.albedo, fileName.toStdString().c_str(), 250);
            m_Material->setTexturePaths(matTexturePaths);

            auto& matTextures = m_Material->getTextures();
            //matTextures.albedo->Release();
            matTextures.albedo = Graphics::RZResourceManager::Get().createTextureFromFile({.name = fileName.toStdString()}, fileName.toStdString());

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

        //---------------------------------------------------------------------------

        void RZEMaterialEditor::on_SpecularTextureSelected()
        {
            auto    fileName = QFileDialog::getOpenFileName(this, "Select Specular Texture", "");
            QPixmap pixmap(fileName);
            QIcon   ButtonIcon(pixmap);
            ui.specTexture->setIcon(ButtonIcon);
            ui.specTexture->setIconSize(QSize(40, 40));

            if (!m_Material && fileName.isEmpty())
                return;

            auto matTexturePaths = m_Material->getTexturePaths();
            memcpy(matTexturePaths.specular, fileName.toStdString().c_str(), 250);
            m_Material->setTexturePaths(matTexturePaths);

            auto& matTextures = m_Material->getTextures();
            //matTextures.albedo->Release();
            matTextures.specular = Graphics::RZResourceManager::Get().createTextureFromFile({.name = fileName.toStdString()}, fileName.toStdString());
            m_Material->setTextures(matTextures);
        }

        void RZEMaterialEditor::on_SpecularIntensity()
        {
            // Get the number from the LineEdit and set the specular float in the MaterialData struct
        }

        //---------------------------------------------------------------------------

        void RZEMaterialEditor::on_NormaTextureSelected()
        {
            auto    fileName = QFileDialog::getOpenFileName(this, "Select Normal Texture", "");
            QPixmap pixmap(fileName);
            QIcon   ButtonIcon(pixmap);
            ui.normalTexture->setIcon(ButtonIcon);
            ui.normalTexture->setIconSize(QSize(40, 40));

            if (!m_Material && fileName.isEmpty())
                return;

            auto matTexturePaths = m_Material->getTexturePaths();
            memcpy(matTexturePaths.normal, fileName.toStdString().c_str(), 250);
            m_Material->setTexturePaths(matTexturePaths);

            auto& matTextures  = m_Material->getTextures();
            matTextures.normal = Graphics::RZResourceManager::Get().createTextureFromFile({.name = fileName.toStdString()}, fileName.toStdString());
            m_Material->setTextures(matTextures);
        }
        //---------------------------------------------------------------------------

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
            auto    fileName = QFileDialog::getOpenFileName(this, "Select Metallic Texture", "");
            QPixmap pixmap(fileName);
            QIcon   ButtonIcon(pixmap);
            ui.metallicTexture->setIcon(ButtonIcon);
            ui.metallicTexture->setIconSize(QSize(40, 40));

            if (!m_Material && fileName.isEmpty())
                return;

            auto matTexturePaths = m_Material->getTexturePaths();
            memcpy(matTexturePaths.metallic, fileName.toStdString().c_str(), 250);
            m_Material->setTexturePaths(matTexturePaths);

            auto& matTextures    = m_Material->getTextures();
            matTextures.metallic = Graphics::RZResourceManager::Get().createTextureFromFile({.name = fileName.toStdString()}, fileName.toStdString());
            m_Material->setTextures(matTextures);
        }
        //---------------------------------------------------------------------------

        void RZEMaterialEditor::on_RoughnessValueSet()
        {
            if (!m_Material)
                return;

            auto matProps           = m_Material->getProperties();
            matProps.roughnessColor = ui.roughnessValue->text().toFloat();
            m_Material->setProperties(matProps);
        }

        void RZEMaterialEditor::on_RoughnessTextureSelected()
        {
            auto    fileName = QFileDialog::getOpenFileName(this, "Select Roughness Texture", "");
            QPixmap pixmap(fileName);
            QIcon   ButtonIcon(pixmap);
            ui.roughnessTexture->setIcon(ButtonIcon);
            ui.roughnessTexture->setIconSize(QSize(40, 40));

            if (!m_Material && fileName.isEmpty())
                return;

            auto matTexturePaths = m_Material->getTexturePaths();
            memcpy(matTexturePaths.roughness, fileName.toStdString().c_str(), 250);
            m_Material->setTexturePaths(matTexturePaths);

            auto& matTextures     = m_Material->getTextures();
            matTextures.roughness = Graphics::RZResourceManager::Get().createTextureFromFile({.name = fileName.toStdString()}, fileName.toStdString());
            m_Material->setTextures(matTextures);
        }
        //---------------------------------------------------------------------------

        void RZEMaterialEditor::on_EmissionIntensity()
        {
            if (!m_Material)
                return;

            auto matProps              = m_Material->getProperties();
            matProps.emissiveIntensity = ui.emissiveIntensity->text().toFloat();
            m_Material->setProperties(matProps);
        }
        //---------------------------------------------------------------------------

        void RZEMaterialEditor::on_AOValueSet()
        {
            if (!m_Material)
                return;

            auto matProps             = m_Material->getProperties();
            matProps.ambientOcclusion = ui.roughnessValue->text().toFloat();
            m_Material->setProperties(matProps);
        }

        void RZEMaterialEditor::on_AOTextureSelected()
        {
            auto    fileName = QFileDialog::getOpenFileName(this, "Select AO Texture", "");
            QPixmap pixmap(fileName);
            QIcon   ButtonIcon(pixmap);
            ui.aoTexture->setIcon(ButtonIcon);
            ui.aoTexture->setIconSize(QSize(40, 40));

            if (!m_Material && fileName.isEmpty())
                return;

            auto matTexturePaths = m_Material->getTexturePaths();
            memcpy(matTexturePaths.ao, fileName.toStdString().c_str(), 250);
            m_Material->setTexturePaths(matTexturePaths);

            auto& matTextures = m_Material->getTextures();
            matTextures.ao    = Graphics::RZResourceManager::Get().createTextureFromFile({.name = fileName.toStdString()}, fileName.toStdString());
            m_Material->setTextures(matTextures);
        }

        //---------------------------------------------------------------------------

        void RZEMaterialEditor::OnUVScaleXChanged()
        {
            if (!m_Material)
                return;

            auto matProps      = m_Material->getProperties();
            matProps.uvScale.x = ui.UVScaleX->text().toInt();
            m_Material->setProperties(matProps);
        }

        void RZEMaterialEditor::OnUVScaleYChanged()
        {
            if (!m_Material)
                return;

            auto matProps      = m_Material->getProperties();
            matProps.uvScale.y = ui.UVScaleY->text().toInt();
            m_Material->setProperties(matProps);
        }

        //---------------------------------------------------------------------------
    }    // namespace Editor
}    // namespace Razix