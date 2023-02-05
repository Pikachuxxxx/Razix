// clang-format off
#include "rzxpch.h"
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

            connect(ui.light_color, SIGNAL(pressed()), this, SLOT(on_light_color_pressed()));
            connect(ui.lightTypeGroup, SIGNAL(buttonClicked(int)), this, SLOT(on_light_type_selected(int)));

            ui.lightTypeGroup->setId(ui.Directional_rb, 0);
            ui.lightTypeGroup->setId(ui.Point_rb, 1);
            ui.lightTypeGroup->setId(ui.Spot_rb, 2);


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

        void RZELightComponentUI::on_light_type_selected(int idx)
        {
            auto& lc = m_Entity.GetComponent<LightComponent>();

            lc.light.setType((Razix::Graphics::LightType)(idx));

            // FIXME: Well we need to use proper 0..1..2.. ids instead of this random mess
            //switch (idx) {
            //    case -2:
            //        lc.light.setType(Razix::Graphics::LightType::DIRECTIONAL);
            //        break;
            //    case -3:
            //        lc.light.setType(Razix::Graphics::LightType::POINT);
            //        break;
            //    case -4:
            //        lc.light.setType(Razix::Graphics::LightType::SPOT);
            //        break;
            //    default:
            //        lc.light.setType(Razix::Graphics::LightType::DIRECTIONAL);
            //        break;
            //}
        }

    }    // namespace Editor
}    // namespace Razix
