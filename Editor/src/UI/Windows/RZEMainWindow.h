#pragma once

#include "generated/ui_RZEMainWindow.h"
#include "generated/ui_RZEViewport.h"

#include "UI/Windows/RZEMaterialEditor.h"

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
            // Project toolbar
            void on_SaveProjectPressed();
            void on_OpenProjectPressed();
            void on_NewProjectPressed();
            // Scene toolbar
            void on_SaveScene();
            void on_LoadScene();
            void on_NewScene();

            void on_RenderAPIChanged(int index);
            void set_TranslateGuizmo();
            void set_RotateGuizmo();
            void set_ScaleGuizmo();
            void update();

        private:
            Ui::MainWindow ui;
            QWidget*       m_Viewport;
            QLabel*        m_FPSLblSB;

        private:
            void SetupToolBars();
            // Toolbar settings
            void create_project_tb();
            void create_scene_tb();
            void create_transform_tb();
            void create_shading_modes_tb();
            void create_game_modes_tb();
            void create_misc_tb();

            void SetupMenu();
            void SetupCreateMenuCommands();
            void SetupWindowsCommands();
            // Menu command functions
            void Windows_MaterialEditor();
            void Create_Entity();
        };
    }    // namespace Editor
}    // namespace Razix
