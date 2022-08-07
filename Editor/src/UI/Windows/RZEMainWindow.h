#pragma once

#include "generated/ui_RZEMainWindow.h"
#include "generated/ui_RZEViewport.h"

#include <QMainWindow>

#include <QToolBar>

namespace Razix {
    namespace Editor {

        class RZEMainWindow : public QMainWindow
        {
            Q_OBJECT
        public:
            RZEMainWindow(QWidget* parent = Q_NULLPTR, Qt::WindowFlags flags = {});
            ~RZEMainWindow() {}

            QWidget*           getViewport() { return m_Viewport; }
            ToolWindowManager* getToolWindowManager() { return ui.toolWindowManager; }

        private:
            Ui::MainWindow ui;
            QWidget*       m_Viewport;
            QToolBar*      renderSettingsTB;
            QToolBar*      m_ProjectSettingsTB;
            QLabel*        m_FPSLblSB;

        public slots:
            void OnSaveProjectPressed();
            void update();
        };
    }    // namespace Editor
}    // namespace Razix
