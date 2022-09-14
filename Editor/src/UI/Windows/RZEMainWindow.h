#pragma once

#include "generated/ui_RZEMainWindow.h"
#include "generated/ui_RZEViewport.h"

#include "Razix/Core/RZApplication.h"

#include <QCloseEvent>
#include <QMainWindow>
#include <QMessageBox>
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

            void closeEvent(QCloseEvent* event)
            {
                QMessageBox::StandardButton resBtn = QMessageBox::question(this, QString("Razix Engine"), QString("Are you sure you want to quit?\n"), QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
                if (resBtn != QMessageBox::Yes) {
                    event->ignore();
                } else {
                    RZApplication::Get().setAppState(AppState::Closing);
                    event->accept();
                }
            }

        signals:
            void OnEntityAddedToScene();

        public slots:
            void OnSaveProjectPressed();
            void update();

        private:
            Ui::MainWindow ui;
            QWidget*       m_Viewport;
            QToolBar*      renderSettingsTB;
            QToolBar*      m_ProjectSettingsTB;
            QLabel*        m_FPSLblSB;

        private:
            void SetupMenu();
            void SetupCreateMenuCommands();
            // Menu command functions
            void Create_Entity();
        };
    }    // namespace Editor
}    // namespace Razix
