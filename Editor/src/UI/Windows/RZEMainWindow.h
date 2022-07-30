#pragma once

#include "generated/ui_RZEMainWindow.h"

#include <QMainWindow>

namespace Razix {
    namespace Editor {

        class RZEMainWindow : public QMainWindow
        {
            Q_OBJECT
        public:
            RZEMainWindow(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = {});
            ~RZEMainWindow() {}

        private:
            Ui::MainWindow ui;
        };
    }    // namespace Editor
}    // namespace Razix
