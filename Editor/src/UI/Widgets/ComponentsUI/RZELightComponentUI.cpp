// clang-format off
#include "rzepch.h"
// clang-format on

#include "RZELightComponentUI.h"

#include <QColorDialog>

#include "Razix/Scene/Components/LightComponent.h"
#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Editor {
        RZELightComponentUI::RZELightComponentUI(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            //connect(ui.light_color, SIGNAL(pressed()), this, SLOT(on_light_color_pressed()));
            connect(ui.lightTypeGroup, SIGNAL(buttonClicked(int)), this, SLOT(on_light_type_selected(int)));

            ui.lightTypeGroup->setId(ui.Directional_rb, 0);
            ui.lightTypeGroup->setId(ui.Point_rb, 1);
            ui.lightTypeGroup->setId(ui.Spot_rb, 2);

            // Default to yellow
            ui.light_color->setStyleSheet("background-color: rgba(255, 255, 255, 255)");
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
            QColor color = QColorDialog::getColor(m_Color);
            m_Color      = color;

            ui.light_color->setStyleSheet("background-color: " + color.name());

            auto& lc = m_Entity.GetComponent<LightComponent>();

            std::cout << glm::to_string(glm::vec3(color.redF(), color.greenF(), color.blueF())) << std::endl;

            lc.light.setColor(glm::vec3(color.redF(), color.greenF(), color.blueF()));
        }

        void RZELightComponentUI::on_light_type_selected(int idx)
        {
            auto& lc = m_Entity.GetComponent<LightComponent>();

            lc.light.setType((LightType)(idx));
        }
    }    // namespace Editor
}    // namespace Razix
