// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZELuaScriptComponentUI.h"

#include <QFileDialog>

namespace Razix {
    namespace Editor {
        RZELuaScriptComponentUI::RZELuaScriptComponentUI(QWidget *parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            // Connect the browse buttons
            connect(ui.Browse, SIGNAL(pressed()), this, SLOT(on_browse_pressed()));
        }

        RZELuaScriptComponentUI::~RZELuaScriptComponentUI()
        {
        }

        void RZELuaScriptComponentUI::on_browse_pressed()
        {
            auto fileName    = QFileDialog::getOpenFileName(this, "Select Lua script", "", tr("Lua File (*.lua)"));
            ui.ScriptFileLineEdit->setText(fileName);

            // Create a component and attach it to the Entity? or Update the script file address and reload/re-compile it
        }

    }    // namespace Editor
}    // namespace Razix
