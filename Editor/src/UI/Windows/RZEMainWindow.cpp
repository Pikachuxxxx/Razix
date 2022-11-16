// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZEMainWindow.h"

#include <QComboBox>
#include <QTimer>

#include "Razix/Core/RZEngine.h"
#include "Razix/Scene/RZEntity.h"

#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Scene/Components/MeshRendererComponent.h"

namespace Razix {
    namespace Editor {

        RZEMainWindow::RZEMainWindow(QWidget* parent /*= Q_NULLPTR*/, Qt::WindowFlags flags /*= {}*/)
            // Create the QApplication here and run it on a separate thread + pass the necessary native window handles to the Razix Application
            : QMainWindow(parent, flags)
        {
            // Link the UI file with this class
            ui.setupUi(this);

            // connect save button to RZApplication::save function

            // Add a label to status bar to show FPS
            QTimer* timer = new QTimer(this);
            connect(timer, SIGNAL(timeout()), this, SLOT(update()));
            timer->start(1000);

            m_FPSLblSB             = new QLabel;
            std::string stylesheet = R"(
                border-color: rgb(147, 147, 147);
                border:1px;
                border-radius:5;b 
                border-style : solid;)";
            m_FPSLblSB->setStyleSheet(stylesheet.c_str());
            QWidget*     widget = new QWidget();    // Is this really needed?
            QGridLayout* layout = new QGridLayout(widget);
            layout->addWidget(m_FPSLblSB, 0, 1, 1, 1, Qt::AlignVCenter | Qt::AlignRight);
            layout->setMargin(0);
            ui.statusbar->addWidget(widget, 1);
            ui.statusbar->setContentsMargins(0, 0, 0, 0);

            // Menu Init
            SetupMenu();

            // Setup ToolBars
            SetupToolBars();
        }

        //------------------------------------------------------------------------------------------------
        void RZEMainWindow::on_save_project_pressed()
        {
            RZApplication::Get().SaveApp();
        }

        void RZEMainWindow::on_render_api_changed(int index)
        {
        }

        void RZEMainWindow::update()
        {
            std::string fps = "FPS : " + std::to_string(Razix::RZEngine::Get().GetStatistics().FramesPerSecond);
            m_FPSLblSB->setText(QString(fps.c_str()));
        }
        //------------------------------------------------------------------------------------------------

        void RZEMainWindow::SetupToolBars()
        {
            create_project_tb();
            create_scene_tb();
            create_transform_tb();
            create_misc_tb();
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
            connect(saveProjectButton, SIGNAL(clicked()), this, SLOT(on_save_project_pressed()));
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

            m_SceneSettingsTB->addWidget(saveButton);
            m_SceneSettingsTB->addWidget(openButton);
            m_SceneSettingsTB->addWidget(newButton);

            this->addToolBar(m_SceneSettingsTB);
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

            transformTB->addWidget(pos);
            transformTB->addWidget(rot);
            transformTB->addWidget(scale);
            transformTB->addWidget(noGuizmo);

            this->addToolBar(transformTB);
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
            QToolBar*   m_RenderSettingsTB = new QToolBar(this);
            QStringList commands           = {"Vulkan", "D3D12", "OpenGL", "Metal"};
            QComboBox*  combo              = new QComboBox(this);
            combo->addItems(commands);
            m_RenderSettingsTB->addWidget(combo);

            m_RenderSettingsTB->addSeparator();

            // Engine/Editor Settings
            // TODO: Add button for various settings (Engine/Editor/Rendering/Lighting etc)
            QPushButton* settingsButton = new QPushButton();
            settingsButton->setIcon(QIcon(":/rzeditor/Razix_Settings_Icon.png"));
            settingsButton->setIconSize(QSize(20, 20));
            m_RenderSettingsTB->addWidget(settingsButton);

            this->addToolBar(m_RenderSettingsTB);

            // Connection for the selection
            connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(on_render_api_changed(int)));
        }

        void RZEMainWindow::SetupMenu()
        {
            // Create Menu commands
            SetupCreateMenuCommands();
        }

        void RZEMainWindow::SetupCreateMenuCommands()
        {
            connect(ui.actionEntity, &QAction::triggered, this, &RZEMainWindow::Create_Entity);
        }

        void RZEMainWindow::Create_Entity()
        {
            // Create an entity
            auto& entity = RZEngine::Get().getSceneManager().getCurrentScene()->createEntity("Entity");
            entity.AddComponent<MeshRendererComponent>(Graphics::MeshPrimitive::Sphere);
            // Update the scene hierarchy panel to re-draw
            emit OnEntityAddedToScene();
        }

    }    // namespace Editor
}    // namespace Razix
