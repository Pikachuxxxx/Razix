#include "RZEMaterialEditor.h"

// FIXME: Adding a pch file for editor as well will resolve this, a QT + engine files together mashed up to reduce build times
#include "Razix/Core/RZRoot.h"
#include "Razix/Graphics/Materials/RZMaterial.h"

#include <QColorDialog>
#include <QFileDialog>

namespace Razix {
    namespace Editor {
        RZEMaterialEditor::RZEMaterialEditor(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            // Connections
            connect(ui.diffuseTexture, SIGNAL(pressed()), this, SLOT(on_diffuse_texture_select()));
        }

        RZEMaterialEditor::~RZEMaterialEditor()
        {
        }

        void RZEMaterialEditor::setEditingMaterial(Razix::Graphics::RZMaterial* material)
        {
            m_Material = material;

            // Set the Material Editor Properties
            ui.materialNameLbl->setText(material->getName().c_str());

            // Diffuse stuff
        }

        void RZEMaterialEditor::on_diffuse_texture_select()
        {
            auto    fileName = QFileDialog::getOpenFileName(this, "Select Diffuse Texture", "");
            QPixmap pixmap(fileName);
            QIcon   ButtonIcon(pixmap);
            ui.diffuseTexture->setIcon(ButtonIcon);
            ui.diffuseTexture->setIconSize(QSize(50, 50));
        }

        void RZEMaterialEditor::on_diffuse_texture_use_checkbox()
        {

        }

        void RZEMaterialEditor::on_diffuse_color()
        {
            QColor color = QColorDialog::getColor();
        }

    }    // namespace Editor
}    // namespace Razix