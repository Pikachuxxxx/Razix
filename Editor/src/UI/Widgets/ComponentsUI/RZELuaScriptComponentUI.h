#pragma once

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

        public slots:
            void on_browse_pressed();

        private:
            Ui::LuaScriptComponent ui;
        };

    }    // namespace Editor
}    // namespace Razix