#pragma once

#include "Razix/Scene/Components/LuaScriptComponent.h"
#include "Razix/Scene/RZEntity.h"

#include <QMouseEvent>
#include <QWidget>

#include "generated/ui_RZELuaScriptComponentUI.h"

namespace Razix {
    namespace Editor {
        class RZELuaScriptComponentUI : public QWidget
        {
            Q_OBJECT

        public:
            RZELuaScriptComponentUI(QWidget *parent = nullptr);
            ~RZELuaScriptComponentUI();

            void setEditingEntity(RZEntity entity);

        public slots:
            void on_browse_pressed();
            void on_reload_pressed();

        private:
            Ui::LuaScriptComponent ui;
            RZEntity               m_Entity;
            LuaScriptComponent     m_LC;
        };

    }    // namespace Editor
}    // namespace Razix