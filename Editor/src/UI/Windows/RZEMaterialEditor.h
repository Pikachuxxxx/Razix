#pragma once

#include <QWidget>

#include "generated/ui_RZEMaterialEditor.h"

namespace Razix {
    namespace Editor {
        class RZEMaterialEditor : public QWidget
        {
            Q_OBJECT

        public:
            RZEMaterialEditor(QWidget *parent = nullptr);
            ~RZEMaterialEditor();

        private:
            Ui::MaterialEditor ui;
        };
    }    // namespace Editor
}    // namespace Razix