#include "RZETransformComponentUI.h"

namespace Razix {
    namespace Editor {
        RZETransformComponentUI::RZETransformComponentUI(QWidget *parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            // Set the name and Icon
            // This is what will be used by the Inspector Window to set for the header title and icon
            this->setWindowTitle("Transform");
            this->setWindowIcon(QIcon(":/rzeditor/transform_icon.png"));
        }

        RZETransformComponentUI::~RZETransformComponentUI()
        {}
    }    // namespace Editor
}    // namespace Razix