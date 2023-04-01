// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEMainWindow.h"

#include <QComboBox>
#include <QPushButton>
#include <QSettings>
#include <QTimer>

#include "Razix/Core/RZEngine.h"
#include "Razix/Scene/RZEntity.h"

#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Scene/Components/MeshRendererComponent.h"

// clang-format off
#include <imgui.h>
#include <ImGui/plugins/ImGuizmo.h>
// clang-format on

namespace Razix {
    namespace Editor {

        constexpr const char* TOOL_WINDOW_LAYOUT_STRING_ID = "tool_window_layout_string_id_1_0_0";

        RZEMainWindow::RZEMainWindow(QWidget* parent /*= Q_NULLPTR*/, Qt::WindowFlags flags /*= {}*/)
            // Create the QApplication here and run it on a separate thread + pass the necessary native window handles to the Razix Application
            : QMainWindow(parent, flags)
        {
            // Link the UI file with this class
            ui.setupUi(this);

            // Set this to every window that one wished to save/restore the state with the LayoutToolWindowManager
            setObjectName(this->windowTitle());

            // Add a label to status bar to show FPS
            QTimer* timer = new QTimer(this);
            connect(timer, SIGNAL(timeout()), this, SLOT(update()));
            timer->start(1000);

            // Create a layout in Status label by adding it to a empty widget
            QWidget*     widget = new QWidget();    // Is this really needed?
            QGridLayout* layout = new QGridLayout(widget);
            layout->setMargin(0);
            ui.statusbar->addWidget(widget, 1);
            ui.statusbar->setContentsMargins(0, 0, 0, 0);

            // Add a FPS label to the progress bar
            m_FPSLblSB             = new QLabel;
            std::string stylesheet = R"(
                border-color: rgb(147, 147, 147);
                border:1px;
                border-radius:5;b 
                border-style : solid;)";
            m_FPSLblSB->setStyleSheet(stylesheet.c_str());
            layout->addWidget(m_FPSLblSB, 0, 1, 1, 1, Qt::AlignVCenter | Qt::AlignRight);

            // Add a progress bar to the Status bar
            //m_StatusProgressBar = new QProgressBar;
            //m_StatusProgressBar->setRange(0, 100);
            //layout->addWidget(m_StatusProgressBar, 0, 0, 1, 1, Qt::AlignVCenter | Qt::AlignRight);

            // Menu Init
            SetupMenu();

            // Setup ToolBars
            SetupToolBars();

            // Restore the layout on start up
            Layout_Restore();
        }

        //------------------------------------------------------------------------------------------------
        void RZEMainWindow::on_SaveProjectPressed()
        {
            RZApplication::Get().SaveApp();
        }

        void RZEMainWindow::on_OpenProjectPressed()
        {
        }

        void RZEMainWindow::on_NewProjectPressed()
        {
        }

        void RZEMainWindow::on_SaveScene()
        {
            RZEngine::Get().getSceneManager().getCurrentScene()->saveScene();
        }

        void RZEMainWindow::on_LoadScene()
        {
            RZEngine::Get().getSceneManager().loadScene();
        }

        void RZEMainWindow::on_NewScene()
        {
        }

        void RZEMainWindow::on_RenderAPIChanged(int index)
        {
        }

        void RZEMainWindow::set_TranslateGuizmo()
        {
            RZApplication::Get().setGuizmoOperation(ImGuizmo::TRANSLATE);
        }

        void RZEMainWindow::set_RotateGuizmo()
        {
            RZApplication::Get().setGuizmoOperation(ImGuizmo::ROTATE);
        }

        void RZEMainWindow::set_ScaleGuizmo()
        {
            RZApplication::Get().setGuizmoOperation(ImGuizmo::SCALE);
        }

        void RZEMainWindow::toggle_WorldLocal()
        {
            if (World_vs_LocalButton->isChecked()) {
                World_vs_LocalButton->setIcon(QIcon(":/rzeditor/local_icon.png"));
                World_vs_LocalButton->setIconSize(QSize(20, 20));

                RZApplication::Get().setGuizmoMode(ImGuizmo::LOCAL);

            } else {
                World_vs_LocalButton->setIcon(QIcon(":/rzeditor/world_icon.png"));
                World_vs_LocalButton->setIconSize(QSize(20, 20));

                RZApplication::Get().setGuizmoMode(ImGuizmo::WORLD);
            }
        }

        void RZEMainWindow::toggle_GridSnap()
        {
            // Read the snap amount from Editor Settings
            if (enableSnapBtn->isChecked())
                RZApplication::Get().setGuizmoSnapAmount(5.0f);
            else
                RZApplication::Get().setGuizmoSnapAmount(0.0f);
        }

        //-----------------------------------------------------------------------------------------------
        void RZEMainWindow::update()
        {
            std::string fps = "FPS : " + std::to_string(Razix::RZEngine::Get().GetStatistics().FramesPerSecond);
            m_FPSLblSB->setText(QString(fps.c_str()));

            if (!Razix::RZEngine::Get().isRZApplicationCreated)
                return;

            //if (RZApplication::Get().getAppState() == AppState::Loading && m_StatusProgressBar->value() < 80)
            //    m_StatusProgressBar->setValue(m_StatusProgressBar->value() + 1);
            //else if (RZApplication::Get().getAppState() == AppState::Loading) {
            //    m_StatusProgressBar->setValue(100);
            //    m_StatusProgressBar->setVisible(false);
            //}
        }
        //------------------------------------------------------------------------------------------------

        void RZEMainWindow::SetupToolBars()
        {
            create_misc_tb();
            create_project_tb();
            create_scene_tb();
            create_transform_tb();
        }

        void RZEMainWindow::create_project_tb()
        {
            QToolBar* m_ProjectSettingsTB = new QToolBar(this);
            // Save Project button
            QPushButton* saveProjectButton = new QPushButton();
            saveProjectButton->setIcon(QIcon(":/rzeditor/save_project.png"));
            saveProjectButton->setIconSize(QSize(20, 20));
            // Open
            QPushButton* openProjectButton = new QPushButton();
            openProjectButton->setIcon(QIcon(":/rzeditor/open_project.png"));
            openProjectButton->setIconSize(QSize(20, 20));
            // New project
            QPushButton* newProjectButton = new QPushButton();
            newProjectButton->setIcon(QIcon(":/rzeditor/new_project.png"));
            newProjectButton->setIconSize(QSize(20, 20));

            m_ProjectSettingsTB->addWidget(saveProjectButton);
            m_ProjectSettingsTB->addWidget(openProjectButton);
            m_ProjectSettingsTB->addWidget(newProjectButton);

            this->addToolBar(m_ProjectSettingsTB);

            // Connection for toolbar
            connect(saveProjectButton, SIGNAL(clicked()), this, SLOT(on_SaveProjectPressed()));
        }

        void RZEMainWindow::create_scene_tb()
        {
            QToolBar* m_SceneSettingsTB = new QToolBar(this);
            // Save Project button
            QPushButton* saveButton = new QPushButton();
            saveButton->setIcon(QIcon(":/rzeditor/save_scene.png"));
            saveButton->setIconSize(QSize(20, 20));
            // Open
            QPushButton* openButton = new QPushButton();
            openButton->setIcon(QIcon(":/rzeditor/open_scene.png"));
            openButton->setIconSize(QSize(20, 20));
            // New project
            QPushButton* newButton = new QPushButton();
            newButton->setIcon(QIcon(":/rzeditor/new_scene.png"));
            newButton->setIconSize(QSize(20, 20));

            // Load next scene (loads first if it's the last scene in a cyclic order)
            QPushButton* nextSceneBtn = new QPushButton;
            nextSceneBtn->setIcon(QIcon("rzeditor/next.png"));

            m_SceneSettingsTB->addWidget(saveButton);
            m_SceneSettingsTB->addWidget(openButton);
            m_SceneSettingsTB->addWidget(newButton);
            m_SceneSettingsTB->addWidget(nextSceneBtn);

            this->addToolBar(m_SceneSettingsTB);

            // Connections for Save/Load/Open scene
            connect(saveButton, SIGNAL(clicked()), this, SLOT(on_SaveScene()));
            connect(nextSceneBtn, SIGNAL(clicked()), this, SLOT(on_LoadScene()));
        }

        void RZEMainWindow::create_transform_tb()
        {
            QToolBar* transformTB = new QToolBar(this);

            QPushButton* pos = new QPushButton();
            pos->setIcon(QIcon(":/rzeditor/Move_Gizmo.png"));
            pos->setIconSize(QSize(20, 20));

            QPushButton* rot = new QPushButton();
            rot->setIcon(QIcon(":/rzeditor/Rotate_Gizmo.png"));
            rot->setIconSize(QSize(20, 20));

            QPushButton* scale = new QPushButton();
            scale->setIcon(QIcon(":/rzeditor/Scale_Gizmo.png"));
            scale->setIconSize(QSize(20, 20));

            // Disable Guizmo
            QPushButton* noGuizmo = new QPushButton();
            noGuizmo->setIcon(QIcon(":/rzeditor/No_Gizmo.png"));
            noGuizmo->setIconSize(QSize(20, 20));

            // Use WORLD space for calculating transformations
            World_vs_LocalButton = new QPushButton;
            World_vs_LocalButton->setCheckable(true);
            World_vs_LocalButton->setIcon(QIcon(":/rzeditor/world_icon.png"));
            World_vs_LocalButton->setIconSize(QSize(20, 20));

            // small line edit for (The snap amount can be set in the Editor Settings menu)
            enableSnapBtn = new QPushButton;
            enableSnapBtn->setCheckable(true);
            enableSnapBtn->setIcon(QIcon(":/rzeditor/grid_snap_icon.png"));
            enableSnapBtn->setIconSize(QSize(20, 20));

            transformTB->addWidget(pos);
            transformTB->addWidget(rot);
            transformTB->addWidget(scale);
            transformTB->addWidget(noGuizmo);
            transformTB->addWidget(World_vs_LocalButton);
            transformTB->addWidget(enableSnapBtn);

            this->addToolBar(transformTB);

            connect(pos, SIGNAL(clicked()), this, SLOT(set_TranslateGuizmo()));
            connect(rot, SIGNAL(clicked()), this, SLOT(set_RotateGuizmo()));
            connect(scale, SIGNAL(clicked()), this, SLOT(set_ScaleGuizmo()));
            connect(World_vs_LocalButton, SIGNAL(pressed()), this, SLOT(toggle_WorldLocal()));
            connect(enableSnapBtn, SIGNAL(pressed()), this, SLOT(toggle_GridSnap()));
        }

        void RZEMainWindow::create_shading_modes_tb()
        {
        }

        void RZEMainWindow::create_game_modes_tb()
        {
        }

        void RZEMainWindow::create_misc_tb()
        {
            // Render API combo box
            QToolBar* m_RenderSettingsTB = new QToolBar(this);

            // Engine/Editor Settings
            // TODO: Add button for various settings windows (Engine/Editor/Rendering/Lighting etc)
            QPushButton* settingsButton = new QPushButton();
            settingsButton->setIcon(QIcon(":/rzeditor/Razix_Settings_Icon.png"));
            settingsButton->setIconSize(QSize(20, 20));
            m_RenderSettingsTB->addWidget(settingsButton);

            m_RenderSettingsTB->addSeparator();

            QStringList commands = {"Vulkan", "D3D12", "OpenGL", "Metal"};
            QComboBox*  combo    = new QComboBox(this);
            combo->addItems(commands);
            m_RenderSettingsTB->addWidget(combo);

            this->addToolBar(m_RenderSettingsTB);

            // Connection for the selection
            connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(on_RenderAPIChanged(int)));
        }

        void RZEMainWindow::SetupMenu()
        {
            // Create Menu commands
            SetupCreateMenuCommands();
            // Setup the Window Commands
            SetupWindowsCommands();
            // Setup the Layout Commands
            SetupLayoutCommands();
        }
        //------------------------------
        // Menu - Create
        void RZEMainWindow::SetupCreateMenuCommands()
        {
            connect(ui.actionEntity, &QAction::triggered, this, &RZEMainWindow::Create_Entity);
        }
        // Create - Action = create Entity
        void RZEMainWindow::Create_Entity()
        {
            // Create an entity
            auto& entity = RZEngine::Get().getSceneManager().getCurrentScene()->createEntity("Entity");
            entity.AddComponent<MeshRendererComponent>(Graphics::MeshPrimitive::Cube);
            // Update the scene hierarchy panel to re-draw
            emit OnEntityAddedToScene();
        }
        //------------------------------
        // Menu - Windows
        void RZEMainWindow::SetupWindowsCommands()
        {
            connect(ui.actionMaterial_Editor, &QAction::triggered, this, &RZEMainWindow::Windows_MaterialEditor);
        }
        // Windows - Action = open/close Material Editor
        void RZEMainWindow::Windows_MaterialEditor()
        {
        }
        //------------------------------
        // Menu - Layout
        void RZEMainWindow::SetupLayoutCommands()
        {
            connect(ui.actionSave_layout, &QAction::triggered, this, &RZEMainWindow::Layout_Save);
            connect(ui.actionRestore_layout, &QAction::triggered, this, &RZEMainWindow::Layout_Restore);
            connect(ui.actionClear_layout, &QAction::triggered, this, &RZEMainWindow::Layout_Clear);
        }
        // Layout - Action = Save layout
        void RZEMainWindow::Layout_Save()
        {
            QSettings layout_settings;
            layout_settings.setValue(TOOL_WINDOW_LAYOUT_STRING_ID, ui.toolWindowManager->saveState());
            layout_settings.setValue("geometry", saveGeometry());
        }
        // Layout - Action = Restore layout
        void RZEMainWindow::Layout_Restore()
        {
            QSettings layout_settings;
            restoreGeometry(layout_settings.value("geometry").toByteArray());
            auto variant_map = layout_settings.value(TOOL_WINDOW_LAYOUT_STRING_ID).toMap();
            ui.toolWindowManager->restoreState(variant_map);
        }
        // Layout - Action = Clear layout
        void RZEMainWindow::Layout_Clear()
        {
            QSettings settings;
            settings.remove("geometry");
            settings.remove(TOOL_WINDOW_LAYOUT_STRING_ID);
        }
    }    // namespace Editor
}    // namespace Razix
