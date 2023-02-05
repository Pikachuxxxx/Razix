// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZELightComponentUI.h"

#include <QColorDialog>

#include "Razix/Scene/RZScene.h"
#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/Components/LightComponent.h"

namespace Razix {
    namespace Editor {
        RZELightComponentUI::RZELightComponentUI(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            connect(ui.light_color, SLOT(pressed()), this, SIGNAL(on_light_color_pressed()));
            // Default to yellow
            ui.light_color->setStyleSheet("background-color: rgba(255, 255, 0, 255)");
        }

        RZELightComponentUI::~RZELightComponentUI()
        {
        }

        void RZELightComponentUI::setEditingEntity(RZEntity entity)
        {
            m_Entity = entity;
        }

        void RZELightComponentUI::on_light_color_pressed()
        {
            QColor color = QColorDialog::getColor();
            ui.light_color->setStyleSheet("background-color: " + color.name());

            QColor rgb = color.toRgb();

            auto& lc = m_Entity.GetComponent<LightComponent>();
            
            lc.light.setColor(glm::vec3(rgb.red() / 255, rgb.green() / 255, rgb.blue() / 255));
        }
    }    // namespace Editor
}    // namespace Razix
