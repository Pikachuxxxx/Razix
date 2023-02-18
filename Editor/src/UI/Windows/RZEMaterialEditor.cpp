#include "RZEMaterialEditor.h"

namespace Razix {
    namespace Editor {
        RZEMaterialEditor::RZEMaterialEditor(QWidget *parent)
            : QWidget(parent)
        {
            ui.setupUi(this);
        }

        RZEMaterialEditor::~RZEMaterialEditor()
        {
        }
    }    // namespace Editor
}    // namespace Razix