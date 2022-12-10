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
            connect(newproject_ui.BrowseBtn, SIGNAL(pressed()), this, SLOT(on_browse_pressed()));
            // Create won't do anything for now just closes
            connect(newproject_ui.CreateBtn, SIGNAL(pressed()), this, SLOT(on_create_pressed()));
            connect(newproject_ui.CancelBtn, SIGNAL(pressed()), this, SLOT(on_cancel_pressed()));

            connect(ui.NewProjectBtn, SIGNAL(clicked()), this, SLOT(on_new_project_pressed()));
            connect(ui.OpenProjectBtn, SIGNAL(clicked()), this, SLOT(on_open_project_pressed()));

            connect(ui.projects_history_list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_select_project(QListWidgetItem*)));

            // Add a default project for debugging
            ui.projects_history_list->addItem(QString("C:/Dev/Game Engines/Razix/Sandbox/RendererTest_ForwardRenderer.razixproject"));
            m_ProjectPath = "C:/Dev/Game Engines/Razix/Sandbox";
            m_ProjectName = "RendererTest_ForwardRenderer";
        }

        RZEProjectBrowser::~RZEProjectBrowser()
        {
        }

        void RZEProjectBrowser::on_select_project(QListWidgetItem* item)
        {
            this->accept();
            this->done(true);
        }

        void RZEProjectBrowser::on_new_project_pressed()
        {
            m_NewProjectDialog->exec();
        }

        void RZEProjectBrowser::on_open_project_pressed()
        {
            auto fileName = QFileDialog::getOpenFileName(this, "Select Razix Project File", "", tr("Razix Project Files (*.razixproject)"));
            ui.projects_history_list->addItem(fileName);
            auto fileNameStr = fileName.toStdString();

            m_ProjectPath      = fileNameStr.substr(0, fileNameStr.find_last_of("\\/"));
            auto   fileNameExt = fileNameStr.substr(fileNameStr.find_last_of("/\\") + 1);
            size_t lastindex   = fileNameExt.find_last_of(".");
            m_ProjectName      = fileNameExt.substr(0, lastindex);
        }

        void RZEProjectBrowser::on_browse_pressed()
        {
            auto fileName = QFileDialog::getExistingDirectory(this, "Select Razix Project Directory");
            newproject_ui.ProjectPath->setText(fileName);
        }

        void RZEProjectBrowser::on_create_pressed()
        {
            m_ProjectName = newproject_ui.ProjectNameLineEdit->text().toStdString();
            m_ProjectPath = newproject_ui.ProjectPath->text().toStdString();
            ui.projects_history_list->addItem(m_ProjectPath.c_str());
            m_NewProjectDialog->accept();
        }

        void RZEProjectBrowser::on_cancel_pressed()
        {
            m_NewProjectDialog->reject();
        }

    }    // namespace Editor
}    // namespace Razix