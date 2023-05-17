#pragma once

#include "generated/ui_RZEMainWindow.h"
#include "generated/ui_RZEViewport.h"

#include "QtADS/DockManager.h"

#include "UI/Windows/RZEMaterialEditor.h"

#include <QCloseEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QProgressBar>
#include <QToolBar>

#include "UI/Windows/RZEWorldRendererSettingsWindow.h"

#include "Razix/Core/RZApplication.h"

namespace Razix {
    namespace Editor {

        class RZEMainWindow : public QMainWindow
        {
            Q_OBJECT
        public:
            RZEMainWindow(QWidget* parent = Q_NULLPTR, Qt::WindowFlags flags = {});
            ~RZEMainWindow() {}

            //ToolWindowManager* getToolWindowManager() { return ui.toolWindowManager; }
            ads::CDockManager* getDockManager() { return m_DockManager; }
            void               addDockableWidget(QWidget* widget, std::string name);

            void closeEvent(QCloseEvent* event)
            {
                QMessageBox::StandardButton resBtn = QMessageBox::question(this, QString("Razix Engine"), QString("Are you sure you want to quit?\n"), QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
                if (resBtn != QMessageBox::Yes) {
                    event->ignore();
                } else {
                    RZApplication::Get().setAppState(AppState::Closing);
                    event->accept();
                    // Save the layout on close
                    //Layout_Save();
                }
            }

            void setProjectPathDir(const std::string& path) { m_ProjectPathDir = path; }

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
            void toggle_WorldLocal();
            void toggle_GridSnap();

            void update();

        private:
            Ui::MainWindow                 ui;
            ads::CDockManager*             m_DockManager;
            std::vector<ads::CDockWidget*> m_DockableWidgets;

            QLabel*       m_FPSLblSB;
            QProgressBar* m_StatusProgressBar;    // TODO: Rename this properly

            QPushButton* World_vs_LocalButton;
            QPushButton* enableSnapBtn;

            // Windows
            RZEWorldRendererSettingsWindow* m_WorldSettingsWindow;

            std::string m_ProjectPathDir;

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
            //------------------------------
            // Menu - Create
            void SetupCreateMenuCommands();
            // Create - Action = create Entity
            void Create_Entity();
            // Create - Import - Model - Action = Import a model file from the disk
            void Create_Import_Model();
            //------------------------------
            // Menu - Windows
            void SetupWindowsCommands();
            // Windows - Action = open/close Material Editor
            void Windows_MaterialEditor();
            // Windows - Action = open/close World Renderer Settings
            void Windows_WorldRendererSettings();
            //------------------------------
            // Menu - Layout
            void SetupLayoutCommands();
            // Layout - Action = Save layout
            void Layout_Save();
            // Layout - Action = Restore layout
            void Layout_Restore();
            // Layout - Action = Clear layout
            void Layout_Clear();
        };
    }    // namespace Editor
}    // namespace Razix
