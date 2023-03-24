// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZESpriteRendererComponentUI.h"

#include <QFileDialog>

namespace Razix {
    namespace Editor {
        RZESpriteRendererComponentUI::RZESpriteRendererComponentUI(QWidget *parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            // connect slots
            connect(ui.sprite, SIGNAL(pressed()), this, SLOT(on_sprite_select_pressed()));
        }

        RZESpriteRendererComponentUI::~RZESpriteRendererComponentUI()
        {
        }

        void RZESpriteRendererComponentUI::on_sprite_select_pressed()
        {
            auto    fileName = QFileDialog::getOpenFileName(this, "Select Sprite", "");
            QPixmap pixmap(fileName);
            QIcon   ButtonIcon(pixmap);
            ui.sprite->setIcon(ButtonIcon);
            ui.sprite->setIconSize(QSize(60, 60));

            // Store the sprite absolute path
            m_SpriteAbsolutePath = fileName.toStdString();
        }

    }    // namespace Editor
}    // namespace Razix