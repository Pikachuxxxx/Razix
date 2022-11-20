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
            void on_select_project(QListWidgetItem* item);
            void on_new_project_pressed();
            void on_open_project_pressed();
            // new project slots
            void on_browse_pressed();
            void on_create_pressed();
            void on_cancel_pressed();

        private:
            Ui::RZEProjectBrowser ui;
            Ui::RZENewProject     newproject_ui;
            QDialog*              m_NewProjectDialog;

            std::string m_ProjectName;
            std::string m_ProjectPath;
        };
    }    // namespace Editor
}    // namespace Razix