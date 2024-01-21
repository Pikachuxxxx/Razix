// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEApplicationMainDockWindow.h"

#include <QComboBox>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QProgressDialog>
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QWidgetAction>

#include <qspdlog/src/qspdlog_toolbar.hpp>

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

        RZEApplicationMainDockWindowCentralWidget::RZEApplicationMainDockWindowCentralWidget(QWidget* parent /*= Q_NULLPTR*/, Qt::WindowFlags flags /*= {}*/)
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

            this->layout()->setContentsMargins(0, 0, 0, 0);

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
            //setAttribute(Qt::WA_TranslucentBackground);    //enable MainWindow to be transparent
            //QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
            //effect->setBlurRadius(5);
            //effect->setOffset(2, 4);
            //effect->setColor(Qt::black);
            //ui.centralwidget->setGraphicsEffect(effect);
            //this->setGraphicsEffect(effect);

            // Menu Init
            SetupMenu();

            // Setup ToolBars
            SetupToolBars();

            // Restore the layout on start up
            Layout_Restore();
        }

        void RZEApplicationMainDockWindowCentralWidget::addDockableWidget(QWidget* widget, std::string name)
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
        void RZEApplicationMainDockWindowCentralWidget::on_SaveProjectPressed()
        {
            RZApplication::Get().SaveApp();
        }

        void RZEApplicationMainDockWindowCentralWidget::on_OpenProjectPressed()
        {
        }

        void RZEApplicationMainDockWindowCentralWidget::on_NewProjectPressed()
        {
        }

        void RZEApplicationMainDockWindowCentralWidget::on_SaveScene()
        {
            RZSceneManager::Get().getCurrentScene()->saveScene();
        }

        void RZEApplicationMainDockWindowCentralWidget::on_LoadScene()
        {
            //RZSceneManager::Get().loadScene();
        }

        void RZEApplicationMainDockWindowCentralWidget::on_NewScene()
        {
        }

        void RZEApplicationMainDockWindowCentralWidget::on_RenderAPIChanged(int index)
        {
        }

        void RZEApplicationMainDockWindowCentralWidget::set_TranslateGuizmo()
        {
            RZApplication::Get().setGuizmoOperation(ImGuizmo::TRANSLATE);
        }

        void RZEApplicationMainDockWindowCentralWidget::set_RotateGuizmo()
        {
            RZApplication::Get().setGuizmoOperation(ImGuizmo::ROTATE);
        }

        void RZEApplicationMainDockWindowCentralWidget::set_ScaleGuizmo()
        {
            RZApplication::Get().setGuizmoOperation(ImGuizmo::SCALE);
        }

        void RZEApplicationMainDockWindowCentralWidget::toggle_WorldLocal()
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

        void RZEApplicationMainDockWindowCentralWidget::toggle_GridSnap()
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
        void RZEApplicationMainDockWindowCentralWidget::update()
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

        void RZEApplicationMainDockWindowCentralWidget::SetupToolBars()
        {
            create_project_tb();
            create_scene_tb();
            create_transform_tb();
            create_misc_tb();
            create_shading_modes_tb();
            create_game_modes_tb();
        }

        void RZEApplicationMainDockWindowCentralWidget::create_project_tb()
        {
            QToolBar* m_ProjectSettingsTB = new QToolBar(this);
            // Save Project button
            QPushButton* saveProjectButton = new QPushButton();
            saveProjectButton->setIcon(QIcon(":/rzeditor/save_project.png"));
            saveProjectButton->setIconSize(QSize(20, 20));
            saveProjectButton->setToolTip("Save the razix project");
            // Open
            QPushButton* openProjectButton = new QPushButton();
            openProjectButton->setIcon(QIcon(":/rzeditor/open_project.png"));
            openProjectButton->setIconSize(QSize(20, 20));
            openProjectButton->setToolTip("Opens existing razix project");
            // New project
            QPushButton* newProjectButton = new QPushButton();
            newProjectButton->setIcon(QIcon(":/rzeditor/new_project.png"));
            newProjectButton->setIconSize(QSize(20, 20));
            newProjectButton->setToolTip("Creates new razix project");

            m_ProjectSettingsTB->addWidget(saveProjectButton);
            m_ProjectSettingsTB->addWidget(openProjectButton);
            m_ProjectSettingsTB->addWidget(newProjectButton);

            m_ProjectSettingsTB->setObjectName("Project Settings Toolbar");

            this->addToolBar(m_ProjectSettingsTB);

            // Connection for toolbar
            connect(saveProjectButton, SIGNAL(clicked()), this, SLOT(on_SaveProjectPressed()));
        }

        void RZEApplicationMainDockWindowCentralWidget::create_scene_tb()
        {
            QToolBar* m_SceneSettingsTB = new QToolBar(this);
            // Save Project button
            QPushButton* saveButton = new QPushButton();
            saveButton->setIcon(QIcon(":/rzeditor/save_scene.png"));
            saveButton->setIconSize(QSize(20, 20));
            saveButton->setToolTip("Saves currently active razix scene");
            // Open
            QPushButton* openButton = new QPushButton();
            openButton->setIcon(QIcon(":/rzeditor/open_scene.png"));
            openButton->setIconSize(QSize(20, 20));
            openButton->setToolTip("opens a razix scene");
            // New project
            QPushButton* newButton = new QPushButton();
            newButton->setIcon(QIcon(":/rzeditor/new_scene.png"));
            newButton->setIconSize(QSize(20, 20));
            newButton->setToolTip("Creates a new razix scene");

            m_SceneSettingsTB->addWidget(saveButton);
            m_SceneSettingsTB->addWidget(openButton);
            m_SceneSettingsTB->addWidget(newButton);

            this->addToolBar(m_SceneSettingsTB);

            m_SceneSettingsTB->setObjectName("Scene Settings Toolbar");

            // Connections for Save/Load/Open scene
            connect(saveButton, SIGNAL(clicked()), this, SLOT(on_SaveScene()));
            connect(openButton, SIGNAL(clicked()), this, SLOT(on_LoadScene()));
        }

        void RZEApplicationMainDockWindowCentralWidget::create_transform_tb()
        {
            QToolBar* transformTB = new QToolBar(this);

            QPushButton* pos = new QPushButton();
            pos->setIcon(QIcon(":/rzeditor/Move_Gizmo.png"));
            pos->setIconSize(QSize(20, 20));
            pos->setToolTip("use the translate Guizmo to move the object");

            QPushButton* rot = new QPushButton();
            rot->setIcon(QIcon(":/rzeditor/Rotate_Gizmo.png"));
            rot->setIconSize(QSize(20, 20));
            rot->setToolTip("use the rotate Guizmo to rotate the object");

            QPushButton* scale = new QPushButton();
            scale->setIcon(QIcon(":/rzeditor/Scale_Gizmo.png"));
            scale->setIconSize(QSize(20, 20));
            scale->setToolTip("use the scale Guizmo to scale the object");

            // Disable Guizmo
            QPushButton* noGuizmo = new QPushButton();
            noGuizmo->setIcon(QIcon(":/rzeditor/No_Gizmo.png"));
            noGuizmo->setIconSize(QSize(20, 20));
            noGuizmo->setToolTip("Disable Guizmo");

            // Use WORLD space for calculating transformations
            World_vs_LocalButton = new QPushButton;
            World_vs_LocalButton->setCheckable(true);
            World_vs_LocalButton->setIcon(QIcon(":/rzeditor/world_icon.png"));
            World_vs_LocalButton->setIconSize(QSize(20, 20));
            World_vs_LocalButton->setToolTip("Change between world and local Guizmo transformation editing");

            // small line edit for (The snap amount can be set in the Editor Settings menu)
            enableSnapBtn = new QPushButton;
            enableSnapBtn->setCheckable(true);
            enableSnapBtn->setIcon(QIcon(":/rzeditor/grid_snap_on.png"));
            enableSnapBtn->setIconSize(QSize(20, 20));
            enableSnapBtn->setToolTip("Whether or not to enable snapping to grid while editing using Guizmo");

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

        void RZEApplicationMainDockWindowCentralWidget::create_shading_modes_tb()
        {
        }

        void RZEApplicationMainDockWindowCentralWidget::create_game_modes_tb()
        {
            QToolBar* gamemodesTB = new QToolBar(this);

            QPushButton* play = new QPushButton();
            play->setIcon(QIcon(":/rzeditor/play.png"));
            play->setIconSize(QSize(20, 20));
            play->setToolTip("Play the editor scene in Game Mode using Razix Game Framework");

            QPushButton* pause = new QPushButton();
            pause->setIcon(QIcon(":/rzeditor/pause.png"));
            pause->setIconSize(QSize(20, 20));
            pause->setToolTip("Pause the scene running in Game Mode");

            QPushButton* nextFrame = new QPushButton();
            nextFrame->setIcon(QIcon(":/rzeditor/next_frame.png"));
            nextFrame->setIconSize(QSize(20, 20));
            nextFrame->setToolTip("Goes to the next frame while in pause mode");

            // TODO: This is will the checked icon for play button itself, dummy for now
            QPushButton* stop = new QPushButton();
            stop->setIcon(QIcon(":/rzeditor/stop.png"));
            stop->setIconSize(QSize(20, 20));
            stop->setToolTip("Stops the Game mode simulation");

            gamemodesTB->addWidget(play);
            gamemodesTB->addWidget(pause);
            gamemodesTB->addWidget(nextFrame);
            gamemodesTB->addWidget(stop);

            this->addToolBar(gamemodesTB);

            gamemodesTB->setObjectName("Game Modes Toolbar");

            //connect(play, SIGNAL(checked()), this, SLOT(gm_PlayOrStop()));
            //connect(pause, SIGNAL(clicked()), this, SLOT(gm_Pause()));
            //connect(nextFrame, SIGNAL(clicked()), this, SLOT(gm_Play()));
        }

        void RZEApplicationMainDockWindowCentralWidget::create_misc_tb()
        {
            // Render API combo box
            QToolBar* m_RenderSettingsTB = new QToolBar(this);

            // Engine/Editor Settings
            // TODO: Add button for various settings windows (Engine/Editor/Rendering/Lighting etc)
            QPushButton* settingsButton = new QPushButton();
            settingsButton->setIcon(QIcon(":/rzeditor/Razix_Settings_Icon.png"));
            settingsButton->setIconSize(QSize(20, 20));
            settingsButton->setToolTip("Razix Engine settings");
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

        void RZEApplicationMainDockWindowCentralWidget::SetupMenu()
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
        void RZEApplicationMainDockWindowCentralWidget::SetupCreateMenuCommands()
        {
            connect(ui.actionEntity, &QAction::triggered, this, &RZEApplicationMainDockWindowCentralWidget::Create_Entity);
            connect(ui.actionModel, &QAction::triggered, this, &RZEApplicationMainDockWindowCentralWidget::Create_Import_Model);
        }
        // Create - Action = create Entity
        void RZEApplicationMainDockWindowCentralWidget::Create_Entity()
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

        void RZEApplicationMainDockWindowCentralWidget::Create_Import_Model()
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
        void RZEApplicationMainDockWindowCentralWidget::SetupWindowsCommands()
        {
            connect(ui.actionMaterial_Editor, &QAction::triggered, this, &RZEApplicationMainDockWindowCentralWidget::Windows_MaterialEditor);

            // Create the m_WorldSettingsWindow and connect it
            m_WorldSettingsWindow = new RZEWorldSettingsWindow;
            // Register it with the DockManager
            addDockableWidget(m_WorldSettingsWindow, "World Settings");
            //m_WorldSettingsWindow->hide();
            connect(ui.actionWorld_Renderer_Settings, &QAction::triggered, this, &RZEApplicationMainDockWindowCentralWidget::Windows_WorldRendererSettings);
        }
        // Windows - Action = open/close Material Editor
        void RZEApplicationMainDockWindowCentralWidget::Windows_MaterialEditor()
        {
        }

        void RZEApplicationMainDockWindowCentralWidget::Windows_WorldRendererSettings()
        {
            if (!m_WorldSettingsWindow->isVisible())
                m_WorldSettingsWindow->show();
        }

        //------------------------------
        // Menu - Layout
        void RZEApplicationMainDockWindowCentralWidget::SetupLayoutCommands()
        {
            connect(ui.actionSave_layout, &QAction::triggered, this, &RZEApplicationMainDockWindowCentralWidget::Layout_Save);
            connect(ui.actionRestore_layout, &QAction::triggered, this, &RZEApplicationMainDockWindowCentralWidget::Layout_Restore);
            connect(ui.actionClear_layout, &QAction::triggered, this, &RZEApplicationMainDockWindowCentralWidget::Layout_Clear);
        }
        // Layout - Action = Save layout
        void RZEApplicationMainDockWindowCentralWidget::Layout_Save()
        {
            QSettings layout_settings("RazixEditorSettings.ini", QSettings::IniFormat);
            layout_settings.setValue("RazixEditor/geometry", this->saveGeometry());
            layout_settings.setValue("RazixEditor/state", this->saveState());
            layout_settings.setValue("RazixEditor/DockingState", m_DockManager->saveState());

            m_DockManager->savePerspectives(layout_settings);
        }
        // Layout - Action = Restore layout
        void RZEApplicationMainDockWindowCentralWidget::Layout_Restore()
        {
            QSettings layout_settings("RazixEditorSettings.ini", QSettings::IniFormat);
            restoreGeometry(layout_settings.value("RazixEditor/geometry").toByteArray());
            restoreState(layout_settings.value("RazixEditor/state").toByteArray());
            m_DockManager->restoreState(layout_settings.value("RazixEditor/DockingState").toByteArray());

            m_DockManager->loadPerspectives(layout_settings);
        }
        // Layout - Action = Clear layout
        void RZEApplicationMainDockWindowCentralWidget::Layout_Clear()
        {
            QSettings settings("RazixEditorSettings.ini", QSettings::IniFormat);
            settings.remove("RazixEditor/geometry");
            settings.remove("RazixEditor/state");
            settings.remove("RazixEditor/DockingState");
        }
        //--------------------------------------------------------------------------
        // RZEAppMainWindow
        //--------------------------------------------------------------------------

        RZEAppMainWindow::RZEAppMainWindow(QWidget* parent /*= nullptr*/)
            : QGoodWindow(parent)
        {
            m_central_widget = new RZEApplicationMainDockWindowCentralWidget(this);
            m_central_widget->setWindowFlags(Qt::Widget);

            m_rze_central_widget = new RZEMainWindowCentralWidget(this);

#ifdef QGOODWINDOW

            //macOS uses global menu bar
    #ifndef Q_OS_MAC
            QMenuBar* menu_bar = m_central_widget->menuBar();

            //Set font of menu bar
            QFont font = menu_bar->font();
            font.setPixelSize(12);
        #ifdef Q_OS_WIN
            font.setFamily("Segoe UI");
        #else
            font.setFamily(qApp->font().family());
        #endif
            menu_bar->setFont(font);

    #endif

            m_rze_central_widget->setTitleBarMenu(menu_bar);
#endif
            qGoodStateHolder->setCurrentThemeDark(QGoodWindow::isSystemThemeDark());

            m_rze_central_widget->setCentralWidget(m_central_widget);
            setCentralWidget(m_rze_central_widget);

            //setWindowIcon(qApp->style()->standardIcon(QStyle::SP_DesktopIcon));
            setWindowTitle("Razix Editor");

            resize(1280, 720);
            move(qApp->primaryScreen()->availableGeometry().center() - rect().center());
        }

        void RZEAppMainWindow::closeEvent(QCloseEvent* event)
        {
            if (m_central_widget)
                m_central_widget->closeEvent(event);
        }
    }    // namespace Editor
}    // namespace Razix
