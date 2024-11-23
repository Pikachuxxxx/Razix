#pragma once

#include <QWidget>

#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

#include "Razix/Scene/Components/LightComponent.h"

#include "generated/ui_RZELightComponentUI.h"

namespace Razix {

    class RZEntity;

    namespace Editor {

        class RZELightComponentUI : public QWidget
        {
            Q_OBJECT

        public:
            RZELightComponentUI(QWidget* parent = nullptr);
            ~RZELightComponentUI();

            void setEditingEntity(RZEntity entity);

        public slots:
            // connect not needed cause on_name_signal was on point on this
            void OnLightColorChanged();
            void OnLightIntensityChanged();
            void OnLightRadiusChanged();
            void OnLightTypeChanged(int idx);

        private:
            QColor             m_Color;
            Ui::LightComponent ui;
            Razix::RZEntity    m_Entity;
        };
    }    // namespace Editor
}    // namespace Razix
