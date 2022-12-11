#include "RZECameraComponentUI.h"

#include <QColorDialog>

namespace Razix {
    namespace Editor {
        RZECameraComponentUI::RZECameraComponentUI(QWidget *parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            //ui.colorPickerPlaceholder = new QColorDialog;
            ui.colorPickerPlaceholder->setVisible(true);
        }

        RZECameraComponentUI::~RZECameraComponentUI()
        {
        }
    }    // namespace Editor
}    // namespace Razix
