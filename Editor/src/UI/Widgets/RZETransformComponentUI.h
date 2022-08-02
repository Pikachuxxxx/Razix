#pragma once

#include <QWidget>

#include "generated/ui_RZETransformComponentUI.h"

namespace Razix {
    namespace Editor {
        class RZETransformComponentUI : public QWidget
        {
            Q_OBJECT

        public:
            RZETransformComponentUI(QWidget *parent = nullptr);
            ~RZETransformComponentUI();

        private:
            Ui::TransformComponent ui;

            // TODO: Add slots for pos, rot, scale edits (enter pressed action) + reset buttons to update the transform component linked with add (add method to link the component and entity it edits)

        };
    }    // namespace Editor
}    // namespace Razix
