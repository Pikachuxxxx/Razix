// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEProjectBrowser.h"

#include <QFileDialog>

namespace Razix {
    namespace Editor {
        RZEProjectBrowser::RZEProjectBrowser(QWidget* parent)
            : QDialog(parent)
        {
            ui.setupUi(this);

            //-------------------------
            // New Project Dialog stuff
            //-------------------------
            m_NewProjectDialog = new QDialog;
            // setup the ui file
            newproject_ui.setupUi(m_NewProjectDialog);
            // Browse Btn hook it up to a QFileDialog to browse a path
            connect(newproject_ui.BrowseBtn, SIGNAL(pressed()), this, SLOT(on_BrowsePressed()));
            // Create won't do anything for now just closes
            connect(newproject_ui.CreateBtn, SIGNAL(pressed()), this, SLOT(on_CreatePressed()));
            connect(newproject_ui.CancelBtn, SIGNAL(pressed()), this, SLOT(on_CancelPressed()));

            connect(ui.NewProjectBtn, SIGNAL(clicked()), this, SLOT(on_NewProjectPressed()));
            connect(ui.OpenProjectBtn, SIGNAL(clicked()), this, SLOT(on_OpenProjectPressed()));

            connect(ui.projects_history_list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_SelectProject(QListWidgetItem*)));

            // Add a default project for debugging
            ui.projects_history_list->addItem(QString("C:/Dev/Game Engines/Razix/Sandbox/RendererTest_PBR.razixproject"));
            m_ProjectPath = "C:/Dev/Game Engines/Razix/Sandbox";
            m_ProjectName = "RendererTest_PBR";
        }

        RZEProjectBrowser::~RZEProjectBrowser()
        {
        }

        void RZEProjectBrowser::on_SelectProject(QListWidgetItem* item)
        {
            this->accept();
            this->done(true);
        }

        void RZEProjectBrowser::on_NewProjectPressed()
        {
            m_NewProjectDialog->exec();
        }

        void RZEProjectBrowser::on_OpenProjectPressed()
        {
            // https://stackoverflow.com/questions/1604440/how-to-set-selected-filter-on-qfiledialog
            // https://doc.qt.io/qt-5/qfiledialog.html#details
            auto fileName = QFileDialog::getOpenFileName(this, "Select Razix Project File", "", tr("Razix Project Files (*.razixproject)"));
            ui.projects_history_list->addItem(fileName);
            auto fileNameStr = fileName.toStdString();

            m_ProjectPath      = fileNameStr.substr(0, fileNameStr.find_last_of("\\/"));
            auto   fileNameExt = fileNameStr.substr(fileNameStr.find_last_of("/\\") + 1);
            size_t lastindex   = fileNameExt.find_last_of(".");
            m_ProjectName      = fileNameExt.substr(0, lastindex);
        }

        void RZEProjectBrowser::on_BrowsePressed()
        {
            auto fileName = QFileDialog::getExistingDirectory(this, "Select Razix Project Directory");
            newproject_ui.ProjectPath->setText(fileName);
        }

        void RZEProjectBrowser::on_CreatePressed()
        {
            m_ProjectName = newproject_ui.ProjectNameLineEdit->text().toStdString();
            m_ProjectPath = newproject_ui.ProjectPath->text().toStdString() + "/";
            ui.projects_history_list->addItem(m_ProjectPath.c_str());
            m_NewProjectDialog->accept();
        }

        void RZEProjectBrowser::on_CancelPressed()
        {
            m_NewProjectDialog->reject();
        }
    }    // namespace Editor
}    // namespace Razix