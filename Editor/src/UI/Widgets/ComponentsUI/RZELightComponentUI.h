#pragma once

#include <QWidget>

#include "generated/ui_RZELightComponentUI.h"

namespace Razix {
    namespace Editor {

        class RZELightComponentUI : public QWidget
        {
            Q_OBJECT

        public:
            RZELightComponentUI(QWidget* parent = nullptr);
            ~RZELightComponentUI();

        private:
            Ui::LightComponent ui;
        };
    }    // namespace Editor
}    // namespace Razix
