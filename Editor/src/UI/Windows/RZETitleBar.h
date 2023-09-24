#pragma once

#include "generated/ui_RZETitleBar.h"
#include <QFrame>

namespace Razix {
    namespace Editor {
        // TODO:
        // [] button Events and window movement
        // [] update project name
        // [] Engine build version label

        class RZETitleBar : public QFrame
        {
            Q_OBJECT

        public:
            RZETitleBar(QWidget* child, QWidget* parent = nullptr);
            ~RZETitleBar();

            void setProjectName(const std::string& projectName) { ui.ProjectName->setText(projectName.c_str()); }
            void setBuildVersion(const std::string& buildVersion) { ui.BuildVersion->setText(buildVersion.c_str()); }

        private slots:
            void on_close_clicked();
            void on_maximum_clicked();
            void on_minimum_clicked();

        protected:
            void mousePressEvent(QMouseEvent* event) override;
            void mouseMoveEvent(QMouseEvent* event) override;
            void mouseReleaseEvent(QMouseEvent* event) override;
            void mouseDoubleClickEvent(QMouseEvent* event) override;
            bool nativeEvent(const QByteArray& eventType, void* message, long* result);
            bool eventFilter(QObject* obj, QEvent* event) override;

        private:
            Ui::TitleBar ui;
            QWidget*     m_MainBody;
            QPointF      m_Position;
            int          borderSize;
        };
    }    // namespace Editor
}    // namespace Razix