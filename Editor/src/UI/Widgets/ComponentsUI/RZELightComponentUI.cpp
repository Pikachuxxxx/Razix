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

            connect(ui.LightColor, SIGNAL(pressed()), this, SLOT(OnLightColorChanged()));
            connect(ui.lightTypeGroup, SIGNAL(buttonClicked(int)), this, SLOT(OnLightTypeChanged(int)));

            connect(ui.Intensity, SIGNAL(returnPressed()), this, SLOT(OnLightIntensityChanged()));
            connect(ui.Radius, SIGNAL(returnPressed()), this, SLOT(OnLightRadiusChanged()));

            ui.lightTypeGroup->setId(ui.Directional_rb, 0);
            ui.lightTypeGroup->setId(ui.Point_rb, 1);
            ui.lightTypeGroup->setId(ui.Spot_rb, 2);

            // Default to yellow
            ui.LightColor->setStyleSheet("background-color: rgba(255, 255, 255, 255)");
        }

        RZELightComponentUI::~RZELightComponentUI()
        {
        }

        void RZELightComponentUI::setEditingEntity(RZEntity entity)
        {
            m_Entity = entity;

            auto& lc = m_Entity.GetComponent<LightComponent>();
            ui.Intensity->setText(std::to_string(lc.light.getIntensity()).c_str());
            ui.Radius->setText(std::to_string(lc.light.getRadius()).c_str());
        }

        void RZELightComponentUI::OnLightColorChanged()
        {
            QColor color = QColorDialog::getColor(m_Color);
            m_Color      = color;

            ui.LightColor->setStyleSheet("background-color: " + color.name());

            auto& lc = m_Entity.GetComponent<LightComponent>();
            lc.light.setColor(glm::vec3(color.redF(), color.greenF(), color.blueF()));
        }

        void RZELightComponentUI::OnLightIntensityChanged()
        {
            auto& lc = m_Entity.GetComponent<LightComponent>();
            lc.light.setIntensity(ui.Intensity->text().toFloat());
        }

        void RZELightComponentUI::OnLightRadiusChanged()
        {
            auto& lc = m_Entity.GetComponent<LightComponent>();
            lc.light.setRadius(ui.Radius->text().toFloat());
        }

        void RZELightComponentUI::OnLightTypeChanged(int idx)
        {
            auto& lc = m_Entity.GetComponent<LightComponent>();

            lc.light.setType((LightType) (idx));
        }
    }    // namespace Editor
}    // namespace Razix
