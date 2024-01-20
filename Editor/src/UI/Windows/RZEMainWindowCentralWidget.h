#pragma once

#include <qgoodwindow.h>

#include "UI/Windows/RZETitleBar.h"

class QGoodWindow;

namespace Razix {
    namespace Editor {

        class RZEMainWindowCentralWidget : public QWidget
        {
            Q_OBJECT

        public:
            RZEMainWindowCentralWidget(QGoodWindow* gw);
            ~RZEMainWindowCentralWidget();

            /** Set the title bar and the central widget unified. */
            void setUnifiedTitleBarAndCentralWidget(bool unified);

            /** Set the central widget of *QGoodCentralWidget*. */
            void setCentralWidget(QWidget* widget);

            void setTitleBarMenu(QMenuBar* menuBar);

        private:
            RZETitleBar* m_Titlebar = nullptr;
            QGoodWindow* m_gw;
            QFrame*      m_frame                                = nullptr;
            QWidget*     m_central_widget_place_holder          = nullptr;
            QWidget*     m_central_widget                       = nullptr;
            bool         m_unified_title_bar_and_central_widget = false;
        };
    }    // namespace Editor
}    // namespace Razix