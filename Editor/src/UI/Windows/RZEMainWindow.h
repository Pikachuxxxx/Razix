#pragma once

#include "generated/ui_RZEMainWindow.h"
#include "generated/ui_RZEViewport.h"

#include <QMainWindow>

namespace Razix {
    namespace Editor {

        class RZEMainWindow : public QMainWindow
        {
            Q_OBJECT
        public:
            RZEMainWindow(QWidget* parent = Q_NULLPTR, Qt::WindowFlags flags = {});
            ~RZEMainWindow() {}

            QWidget* getViewport() { return m_Viewport; }

        private:
            Ui::MainWindow ui;
            QWidget*       m_Viewport;
        };
    }    // namespace Editor
}    // namespace Razix
