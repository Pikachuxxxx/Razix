#pragma once

#include "generated/ui_RZETitleBar.h"
#include <QFrame>

namespace Razix {
    namespace Editor {
        class RZETitleBar : public QFrame
        {
            Q_OBJECT

        public:
            RZETitleBar(QWidget* child, QWidget* parent = nullptr);
            ~RZETitleBar();

        private:
            Ui::TitleBar ui;
            QWidget*     m_MainBody;
        };
    }    // namespace Editor
}    // namespace Razix