// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZECameraComponentUI.h"

#include "Razix/Scene/Components/CameraComponent.h"

#include <QColorDialog>

namespace Razix {
    namespace Editor {
        RZECameraComponentUI::RZECameraComponentUI(QWidget *parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            connect(ui.bg_color, SIGNAL(pressed()), this, SLOT(On_SetCameraBgColor()));
        }

        RZECameraComponentUI::~RZECameraComponentUI()
        {
        }

        void RZECameraComponentUI::setEditingEntity(Razix::RZEntity entity)
        {
            m_Entity = entity;
        }

        void RZECameraComponentUI::On_SetCameraBgColor()
        {
            QColor color = QColorDialog::getColor(m_BgColor);
            m_BgColor    = color;

            ui.bg_color->setStyleSheet("background-color: " + color.name());

            m_Entity.GetComponent<CameraComponent>().Camera.setBgColor(glm::vec4(color.redF(), color.greenF(), color.blueF(), color.alphaF()));
        }

    }    // namespace Editor
}    // namespace Razix
