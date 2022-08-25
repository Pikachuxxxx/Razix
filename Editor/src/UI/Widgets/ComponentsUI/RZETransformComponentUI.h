#pragma once

#include <glm/glm.hpp>
#include <sol/sol.hpp>

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZRoot.h"
#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"
#include "Razix/Scene/Components/TransformComponent.h"

#include <QWidget>

#include "generated/ui_RZETransformComponentUI.h"

namespace Razix {
    namespace Editor {
        // TODO: Implement ImGui like Drag float feature for the line edit
        class RZETransformComponentUI : public QWidget
        {
            Q_OBJECT

        public:
            RZETransformComponentUI(QWidget *parent = nullptr);
            ~RZETransformComponentUI();

            void setEditingEntity(RZEntity entity);

        public slots:
            void OnPosXEdited();
            void OnPosYEdited();
            void OnPosZEdited();

            void OnRotXEdited();
            void OnRotYEdited();
            void OnRotZEdited();

            void OnScaleXEdited();
            void OnScaleYEdited();
            void OnScaleZEdited();

            // TODO: Add slots for buttons to reset the values to 0 (very low priority implementation)

        private:
            Ui::TransformComponent ui;
            RZEntity               m_Entity;
            TransformComponent     m_TC;

            // TODO: Add slots for pos, rot, scale edits (enter pressed action) + reset buttons to update the transform component linked with add (add method to link the component to it's entity)
        };
    }    // namespace Editor
}    // namespace Razix
