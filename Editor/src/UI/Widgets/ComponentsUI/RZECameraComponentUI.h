#pragma once

#include <QColorDialog>
#include <QWidget>

#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

#include "generated/ui_RZECameraComponentUI.h"

namespace Razix {

    class RZEntity;

    namespace Editor {
        class RZECameraComponentUI : public QWidget
        {
            Q_OBJECT

        public:
            RZECameraComponentUI(QWidget *parent = nullptr);
            ~RZECameraComponentUI();

            void setEditingEntity(Razix::RZEntity entity);

        public slots:
            void On_SetCameraBgColor();

        private:
            Ui::CameraComponent ui;
            QColor              m_BgColor;
            Razix::RZEntity     m_Entity;
        };
    }    // namespace Editor
}    // namespace Razix
