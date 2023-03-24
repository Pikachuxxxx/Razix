// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEMaterialEditor.h"

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

            // Connections
            connect(ui.diffuseTexture, SIGNAL(pressed()), this, SLOT(on_DiffuseTextureSelect()));
            connect(ui.diffuseColor, SIGNAL(pressed()), this, SLOT(on_DiffuseColor()));
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
        }

        void RZEMaterialEditor::on_DiffuseTextureSelect()
        {
            auto    fileName = QFileDialog::getOpenFileName(this, "Select Diffuse Texture", "");
            QPixmap pixmap(fileName);
            QIcon   ButtonIcon(pixmap);
            ui.diffuseTexture->setIcon(ButtonIcon);
            ui.diffuseTexture->setIconSize(QSize(50, 50));
        }

        void RZEMaterialEditor::onDiffuseTextureUseCheckbox()
        {
        }

        void RZEMaterialEditor::on_DiffuseColor()
        {
            QColor color   = QColorDialog::getColor(m_DiffuseColor);
            m_DiffuseColor = color;

            ui.diffuseColor->setStyleSheet("background-color: " + color.name());

            std::cout << glm::to_string(glm::vec3(color.redF(), color.greenF(), color.blueF())) << std::endl;

            auto matProps        = m_Material->getProperties();
            matProps.albedoColor = glm::vec3(color.redF(), color.greenF(), color.blueF());
            m_Material->setProperties(matProps);
        }
    }    // namespace Editor
}    // namespace Razix