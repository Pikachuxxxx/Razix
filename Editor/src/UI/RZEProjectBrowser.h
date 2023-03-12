#pragma once

#include <string>

#include <QDialog>

#include "generated/ui_RZENewProject.h"
#include "generated/ui_RZEProjectBrowser.h"

namespace Razix {
    namespace Editor {
        class RZEProjectBrowser : public QDialog
        {
            Q_OBJECT

        public:
            RZEProjectBrowser(QWidget *parent = nullptr);
            ~RZEProjectBrowser();

            const std::string& getProjectName() const { return m_ProjectName; }
            const std::string& getProjectPath() const { return m_ProjectPath; }
        public slots:
            void on_SelectProject(QListWidgetItem* item);
            void on_NewProjectPressed();
            void on_OpenProjectPressed();
            // new project slots
            void on_BrowsePressed();
            void on_CreatePressed();
            void on_CancelPressed();

        private:
            Ui::RZEProjectBrowser ui;
            Ui::RZENewProject     newproject_ui;
            QDialog*              m_NewProjectDialog;

            std::string m_ProjectName;
            std::string m_ProjectPath;
        };
    }    // namespace Editor
}    // namespace Razix