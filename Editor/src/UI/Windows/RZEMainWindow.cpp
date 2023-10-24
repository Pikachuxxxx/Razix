// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEMainWindow.h"

#include <QComboBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QWidgetAction>

#include "qspdlog/src/qspdlog_toolbar.hpp"

#include "Razix/Core/RZEngine.h"
#include "Razix/Scene/RZEntity.h"

#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Scene/Components/HierarchyComponent.h"
#include "Razix/Scene/Components/MeshRendererComponent.h"
#include "Razix/Scene/Components/TransformComponent.h"

// Razix Tools
#include "exporter/MeshExporter.h"
#include "importer/MeshImporter.h"

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

            // Create the dock manager after the ui is setup. Because the
            // parent parameter is a QMainWindow the dock manager registers
            // itself as the central widget as such the ui must be set up first.
            m_DockManager = new ads::CDockManager(this);
            m_DockManager->setStyleSheet("");

            // Qt spdlog console widget
            m_ConsoleLogWidget = new QSpdLog;
            m_ConsoleLogWidget->setObjectName("Razix Console Log");
            m_ConsoleLogWidget->setMaxEntries(500);
            QSpdLogToolBar* consoleToolBar = new QSpdLogToolBar();
            m_ConsoleLogWidget->registerToolbar(consoleToolBar);
            dynamic_cast<QVBoxLayout*>(m_ConsoleLogWidget->layout())->insertWidget(0, consoleToolBar);

            addDockableWidget(m_ConsoleLogWidget, "Console Log");

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
            ui.statusbar->setMinimumHeight(25);

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

            // Custom TitleBar
            // Remove the TitleBar first
            //setWindowFlag(Qt::FramelessWindowHint);
            // Add events to handle moving and resizing
            // Add Custom title bar buttons, labels etc.

            // Menu Init
            SetupMenu();

            // Setup ToolBars
            SetupToolBars();

            // Restore the layout on start up
            Layout_Restore();
        }

        void RZEMainWindow::addDockableWidget(QWidget* widget, std::string name)
        {
            // Create a dock widget with the title Label 1 and set the created label
            // as the dock widget content
            ads::CDockWidget* DockWidget = new ads::CDockWidget(name.c_str());
            DockWidget->setWidget(widget);

            m_DockableWidgets.push_back(DockWidget);

            ui.menuWindows->addAction(DockWidget->toggleViewAction());
            m_DockManager->addDockWidget(ads::AllDockAreas, DockWidget);
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
            RZSceneManager::Get().getCurrentScene()->saveScene();
        }

        void RZEMainWindow::on_LoadScene()
        {
            RZSceneManager::Get().loadScene();
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
            if (enableSnapBtn->isChecked()) {
                enableSnapBtn->setIcon(QIcon(":/rzeditor/grid_snap_on.png"));
                RZApplication::Get().setGuizmoSnapAmount(5.0f);
            } else {
                enableSnapBtn->setIcon(QIcon(":/rzeditor/grid_snap_off.png"));
                RZApplication::Get().setGuizmoSnapAmount(0.0f);
            }
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

            m_ProjectSettingsTB->setObjectName("Project Settings Toolbar");

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
            nextSceneBtn->setIcon(QIcon(":/rzeditor/styles/icons/progress-pattern.png"));

            m_SceneSettingsTB->addWidget(saveButton);
            m_SceneSettingsTB->addWidget(openButton);
            m_SceneSettingsTB->addWidget(newButton);
            m_SceneSettingsTB->addWidget(nextSceneBtn);

            this->addToolBar(m_SceneSettingsTB);

            m_SceneSettingsTB->setObjectName("Scene Settings Toolbar");

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
            enableSnapBtn->setIcon(QIcon(":/rzeditor/grid_snap_on.png"));
            enableSnapBtn->setIconSize(QSize(20, 20));

            transformTB->addWidget(pos);
            transformTB->addWidget(rot);
            transformTB->addWidget(scale);
            transformTB->addWidget(noGuizmo);
            transformTB->addWidget(World_vs_LocalButton);
            transformTB->addWidget(enableSnapBtn);

            this->addToolBar(transformTB);

            transformTB->setObjectName("Transform Settings Toolbar");

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

            m_RenderSettingsTB->setObjectName("Render Settings Toolbar");

            // Connection for the selection
            connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(on_RenderAPIChanged(int)));
        }

        void RZEMainWindow::SetupMenu()
        {
            QWidgetAction* labelAct = new QWidgetAction(ui.menubar);
            QLabel*        label    = new QLabel(" ", ui.menubar);
            QPixmap        mypix(":/rzeditor/RazixLogo64.png");
            label->setPixmap(mypix);
            labelAct->setDefaultWidget(label);
            ui.menubar->addAction(labelAct);

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
            connect(ui.actionModel, &QAction::triggered, this, &RZEMainWindow::Create_Import_Model);
        }
        // Create - Action = create Entity
        void RZEMainWindow::Create_Entity()
        {
            // Create an entity
            auto entity = RZSceneManager::Get().getCurrentScene()->createEntity("Entity");
            //entity.AddComponent<MeshRendererComponent>(Graphics::MeshPrimitive::Cube);
            // Update the scene hierarchy panel to re-draw
            emit OnEntityAddedToScene();
        }

        static void CreateEntityHierarchy(const Razix::Tool::AssetPacker::Node* parentNode, Razix::RZEntity& parentEntity, const std::string& rootMeshName, QProgressDialog* progressBar)
        {
            for (size_t i = 0; i < parentNode->numChildren; i++) {
                auto childNode = parentNode->children[i];
                progressBar->setValue(progressBar->value() + 1);
                auto lbl = std::string("Importing rzmesh...") + std::string(childNode.name);
                progressBar->setLabelText(lbl.c_str());
                auto scene                                                        = RZSceneManager::Get().getCurrentScene();
                auto childEntity                                                  = scene->createEntity(childNode.name);
                childEntity.GetComponent<Razix::TransformComponent>().Translation = childNode.translation;
                auto& hc                                                          = childEntity.AddComponent<Razix::HierarchyComponent>(parentEntity);
                hc.OnConstruct(scene->getRegistry(), childEntity);

                // TODO: Add a progress bar (test running on a separate thread)
                // Add a Mesh Renderer / Skinned Mesh Renderer component as needed
                // TODO: Improve how meshes path is resolved, AssetPackerTool should give proper Relative paths!!!!
                if (std::string(childNode.nodeType) == "$MESH") {
                    std::string meshFilePath = "//Assets/Cache/Meshes/" + rootMeshName + "/" + rootMeshName + "_" + childNode.name + ".rzmesh";
                    childEntity.AddComponent<MeshRendererComponent>(meshFilePath);
                }

                if (childNode.numChildren)
                    CreateEntityHierarchy(&childNode, childEntity, rootMeshName, progressBar);
            }
        }

        void RZEMainWindow::Create_Import_Model()
        {
            // TODO: Add a UI for before and after import launch results and stuff
            auto fileName = QFileDialog::getOpenFileName(this, "Select Model File to load", "", tr("Model files (*.glb *.gltf *.obj *.dae)"));
            if (!fileName.isEmpty()) {
                // Progress Dialog
                QProgressDialog* progressBar = new QProgressDialog("Importing mesh...", "Cancel", 0, 100);
                progressBar->show();
                progressBar->setWindowModality(Qt::WindowModal);
                progressBar->setMinimumDuration(0);

                // Results of the mesh import
                Razix::Tool::AssetPacker::MeshImportResult import_result;
                // Mesh import options
                Razix::Tool::AssetPacker::MeshImportOptions import_options{};

                Razix::Tool::AssetPacker::MeshImporter* importer = new Razix::Tool::AssetPacker::MeshImporter;

                bool result = importer->importMesh(fileName.toStdString().c_str(), import_result, import_options);
                if (!result) {
                    RAZIX_ERROR("[ERROR!] Mesh Import failed for file {0}", fileName.toStdString());
                    return;
                }

                progressBar->setLabelText("Mesh Imported!");
                progressBar->setValue(25);

                progressBar->setLabelText("Exporting to custom format...");

                // Export Options
                Razix::Tool::AssetPacker::MeshExportOptions export_options{};
                export_options.assetsOutputDirectory = m_ProjectPathDir + "/Assets/";
                // Exporter
                Razix::Tool::AssetPacker::MeshExporter exporter;
                result = exporter.exportMesh(import_result, export_options);
                if (!result) {
                    RAZIX_ERROR("[ERROR!] Mesh Export Failed failed for file {0} to path {1}", fileName.toStdString(), export_options.assetsOutputDirectory);
                    return;
                }

                progressBar->setValue(50);
                progressBar->setLabelText("Importing into Engine and creatingHierarchy!");

                // Create the Hierarchy, pass the stuff to engine and emit a signal to re-paint the scene hierarchy panel
                auto rootNode = importer->getRootNode();
                // Add the root node first
                auto rootEntity                                                  = RZSceneManager::Get().getCurrentScene()->createEntity(rootNode->name);
                rootEntity.GetComponent<Razix::TransformComponent>().Translation = rootNode->translation;
                auto& hc                                                         = rootEntity.AddComponent<Razix::HierarchyComponent>();
                hc.OnConstruct(RZSceneManager::Get().getCurrentScene()->getRegistry(), rootEntity);

                // Now stop the other thread first from rendering before we issue resize commands
                std::lock_guard<std::mutex> lk(RZApplication::m);
                RZApplication::ready_for_execution = false;
                //RAZIX_INFO("Triggering worker thread to halt execution ::::");
                RZApplication::halt_execution.notify_one();

                CreateEntityHierarchy(rootNode, rootEntity, std::string(rootNode->name), progressBar);

                // Now stop the other thread first from rendering before we issue resize commands
                RZApplication::ready_for_execution = true;
                //RAZIX_INFO("Triggering worker thread to halt execution ::::");
                RZApplication::halt_execution.notify_one();

                // update scene hierarchy panel
                emit OnEntityAddedToScene();

                progressBar->setValue(100);
                progressBar->setLabelText("Done!");

                progressBar->done(100);
                delete progressBar;
                delete importer;
            }
        }

        //------------------------------
        // Menu - Windows
        void RZEMainWindow::SetupWindowsCommands()
        {
            connect(ui.actionMaterial_Editor, &QAction::triggered, this, &RZEMainWindow::Windows_MaterialEditor);

            // Create the m_WorldSettingsWindow and connect it
            m_WorldSettingsWindow = new RZEWorldSettingsWindow;
            // Register it with the DockManager
            addDockableWidget(m_WorldSettingsWindow, "World Settings");
            //m_WorldSettingsWindow->hide();
            connect(ui.actionWorld_Renderer_Settings, &QAction::triggered, this, &RZEMainWindow::Windows_WorldRendererSettings);
        }
        // Windows - Action = open/close Material Editor
        void RZEMainWindow::Windows_MaterialEditor()
        {
        }

        void RZEMainWindow::Windows_WorldRendererSettings()
        {
            if (!m_WorldSettingsWindow->isVisible())
                m_WorldSettingsWindow->show();
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
            QSettings layout_settings("RazixEditorSettings.ini", QSettings::IniFormat);
            layout_settings.setValue("RazixEditor/geometry", this->saveGeometry());
            layout_settings.setValue("RazixEditor/state", this->saveState());
            layout_settings.setValue("RazixEditor/DockingState", m_DockManager->saveState());

            m_DockManager->savePerspectives(layout_settings);
        }
        // Layout - Action = Restore layout
        void RZEMainWindow::Layout_Restore()
        {
            QSettings layout_settings("RazixEditorSettings.ini", QSettings::IniFormat);
            restoreGeometry(layout_settings.value("RazixEditor/geometry").toByteArray());
            restoreState(layout_settings.value("RazixEditor/state").toByteArray());
            m_DockManager->restoreState(layout_settings.value("RazixEditor/DockingState").toByteArray());

            m_DockManager->loadPerspectives(layout_settings);
        }
        // Layout - Action = Clear layout
        void RZEMainWindow::Layout_Clear()
        {
            QSettings settings("RazixEditorSettings.ini", QSettings::IniFormat);
            settings.remove("RazixEditor/geometry");
            settings.remove("RazixEditor/state");
            settings.remove("RazixEditor/DockingState");
        }
    }    // namespace Editor
}    // namespace Razix
