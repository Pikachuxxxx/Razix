#pragma once

#include <QWidget>

#include "generated/ui_RZEViewport.h"

namespace Razix {
    namespace Editor {
        class RZEViewport : public QWidget
        {
            Q_OBJECT

        public:
            RZEViewport(QWidget *parent = nullptr);
            ~RZEViewport();

            private:
            Ui::Viewport ui;
        };
    }    // namespace Editor
}    // namespace Razix