#pragma once

#include <QWidget>
#include "generated/ui_RZECameraComponentUI.h"

namespace Razix {
    namespace Editor {
        class RZECameraComponentUI : public QWidget
        {
            Q_OBJECT

        public:
            RZECameraComponentUI(QWidget *parent = nullptr);
            ~RZECameraComponentUI();

        private:
            Ui::CameraComponent ui;
        };
    }    // namespace Editor
}    // namespace Razix
