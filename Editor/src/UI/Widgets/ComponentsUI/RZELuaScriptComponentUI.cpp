// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZELuaScriptComponentUI.h"

#include <QFileDialog>

namespace Razix {
    namespace Editor {
        RZELuaScriptComponentUI::RZELuaScriptComponentUI(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            // Connect the browse buttons
            connect(ui.Browse, SIGNAL(pressed()), this, SLOT(on_browse_pressed()));
            connect(ui.Reload, SIGNAL(pressed()), this, SLOT(on_reload_pressed()));
        }

        RZELuaScriptComponentUI::~RZELuaScriptComponentUI()
        {
        }

        void RZELuaScriptComponentUI::setEditingEntity(RZEntity entity)
        {
            m_Entity = entity;
            auto& lc = m_Entity.GetComponent<LuaScriptComponent>();
            m_LC     = lc;

            ui.ScriptFileLineEdit->setText(m_LC.getScriptFilePath().c_str());
        }

        void RZELuaScriptComponentUI::on_browse_pressed()
        {
            auto fileName = QFileDialog::getOpenFileName(this, "Select Lua script", "", tr("Lua File (*.lua)"));
            ui.ScriptFileLineEdit->setText(fileName);

            m_LC.loadScript(fileName.toStdString());

            auto& lc = m_Entity.GetComponent<LuaScriptComponent>();
            lc       = m_LC;
        }

        void RZELuaScriptComponentUI::on_reload_pressed()
        {
            m_LC.loadScript(ui.ScriptFileLineEdit->text().toStdString());

            auto& lc = m_Entity.GetComponent<LuaScriptComponent>();
            lc       = m_LC;
        }
    }    // namespace Editor
}    // namespace Razix
