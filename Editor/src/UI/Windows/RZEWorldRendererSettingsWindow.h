#pragma once

#include "generated/ui_RZEWorldRendererSettingsWindow.h"
#include <QWidget>

namespace Razix {
    namespace Editor {

        class RZEWorldRendererSettingsWindow : public QWidget
        {
            Q_OBJECT

        public:
            RZEWorldRendererSettingsWindow(QWidget *parent = nullptr);
            ~RZEWorldRendererSettingsWindow();

        public slots:
            void On_SetFilterRadius();
            void On_SetStrength();

        private:
            Ui::WorldRendererSettings ui;
        };
    }    // namespace Editor
}    // namespace Razix