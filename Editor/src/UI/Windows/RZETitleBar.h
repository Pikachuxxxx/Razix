#pragma once

#include "generated/ui_RZETitleBar.h"

#include <QFrame>

#include <qgoodwindow.h>

class QGoodWindow;

namespace Razix {
    namespace Editor {

        class RZETitleBar : public QFrame
        {
            Q_OBJECT

        public:
            RZETitleBar(QGoodWindow* goodWindow, QWidget* parent = nullptr);
            ~RZETitleBar();

            void setProjectName(const std::string& projectName) { ui.ProjectName->setText(projectName.c_str()); }
            void setBuildVersion(const std::string& buildVersion) { ui.BuildVersion->setText(buildVersion.c_str()); }
            void setMenuBar(QMenuBar* menuBar);

        public slots:
            void captionButtonStateChanged(const QGoodWindow::CaptionButtonState& state);

        private:
            Ui::TitleBar ui;
            QGoodWindow* m_GoodWindow;
        };
    }    // namespace Editor
}    // namespace Razix