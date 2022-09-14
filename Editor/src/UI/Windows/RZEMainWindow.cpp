// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZEMainWindow.h"

#include <QComboBox>
#include <QTimer>

#include "Razix/Core/RZEngine.h"
#include "Razix/Scene/RZEntity.h"

#include "Razix/Scene/Components/MeshRendererComponent.h"
#include "Razix/Graphics/RZMeshFactory.h"

namespace Razix {
    namespace Editor {

        RZEMainWindow::RZEMainWindow(QWidget* parent /*= Q_NULLPTR*/, Qt::WindowFlags flags /*= {}*/)
            // Create the QApplication here and run it on a separate thread + pass the necessary native window handles to the Razix Application
            : QMainWindow(parent, flags)
        {
            // Link the UI file with this class
            ui.setupUi(this);

            //ui.toolWindowManager->setToolWindowCreateCallback([this](const QString &objectName) -> QWidget * {
            //    std::cout << "Window added : " << objectName.toStdString() << std::endl;
            //    return
            //});

            renderSettingsTB     = new QToolBar(this);
            QStringList commands = {"OpenGL", "Vulkan", "DX12"};
            QComboBox*  combo    = new QComboBox(this);
            combo->addItems(commands);
            renderSettingsTB->addWidget(combo);
            renderSettingsTB->addSeparator();
            renderSettingsTB->addWidget(new QPushButton("Play"));

            //this->addToolBarBreak();

            m_ProjectSettingsTB = new QToolBar(this);

            // TODO: add this to a new class derived from QToolBar to make code more readable and less cluster fucked!
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
            this->addToolBar(renderSettingsTB);

            // connect save button to RZApplication::save function
            connect(saveProjectButton, SIGNAL(clicked()), this, SLOT(OnSaveProjectPressed()));

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
        }

        void RZEMainWindow::OnSaveProjectPressed()
        {
            RZApplication::Get().SaveApp();
        }

        void RZEMainWindow::update()
        {
            std::string fps = "FPS : " + std::to_string(Razix::RZEngine::Get().GetStatistics().FramesPerSecond);
            m_FPSLblSB->setText(QString(fps.c_str()));
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
            auto& entity = RZEngine::Get().getSceneManager().getCurrentScene()->createEntity("Random Entity");
            entity.AddComponent<MeshRendererComponent>(Graphics::MeshPrimitive::Plane);
            // Update the scene hierarchy panel to re-draw
            emit OnEntityAddedToScene();
        }
    }    // namespace Editor
}    // namespace Razix
