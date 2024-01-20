// clang-format off
#include "rzepch.h"
// clang-format on

#include "RZEMainWindowCentralWidget.h"

#include "Razix/Core/RZApplication.h"

#include <qgoodwindow.h>

namespace Razix {
    namespace Editor {
        RZEMainWindowCentralWidget::RZEMainWindowCentralWidget(QGoodWindow* gw)
            : QWidget(gw)
        {
            m_gw = gw;

            m_gw->installEventFilter(this);

            m_central_widget_place_holder = new QWidget(this);

#ifdef QGOODWINDOW

            m_Titlebar = new RZETitleBar(gw);
            m_Titlebar->installEventFilter(this);

            m_gw->setTitleBarHeight(m_Titlebar->height());

            // Frame to hold the MainWindow.ui I suppose
            m_frame = new QFrame(this);
            m_frame->setObjectName("GoodFrame");

            setUnifiedTitleBarAndCentralWidget(false);

            QVBoxLayout* central_layout = new QVBoxLayout(this);
            central_layout->setContentsMargins(0, 0, 0, 0);
            central_layout->setSpacing(0);
            central_layout->addWidget(m_frame);
#endif
        }

        RZEMainWindowCentralWidget::~RZEMainWindowCentralWidget()
        {
        }

        void RZEMainWindowCentralWidget::setUnifiedTitleBarAndCentralWidget(bool unified)
        {
#ifdef QGOODWINDOW
            if (m_frame->layout())
                delete m_frame->layout();

            if (!unified) {
                m_unified_title_bar_and_central_widget = false;

                QVBoxLayout* main_layout = new QVBoxLayout(m_frame);
                main_layout->setContentsMargins(0, 0, 0, 0);
                main_layout->setSpacing(0);
                main_layout->addWidget(m_Titlebar);
                main_layout->addWidget(m_central_widget_place_holder);
            } else {
                m_unified_title_bar_and_central_widget = true;

                QStackedLayout* main_layout = new QStackedLayout(m_frame);
                main_layout->setStackingMode(QStackedLayout::StackAll);
                main_layout->setContentsMargins(0, 0, 0, 0);
                main_layout->setSpacing(0);
                main_layout->addWidget(m_Titlebar);
                main_layout->addWidget(m_central_widget_place_holder);
            }
#else
            Q_UNUSED(unified)
#endif
        }

        void RZEMainWindowCentralWidget::setCentralWidget(QWidget* widget)
        {
            if (!m_central_widget_place_holder)
                return;

            m_central_widget = widget;

            if (m_central_widget_place_holder->layout())
                delete m_central_widget_place_holder->layout();

            QGridLayout* layout = new QGridLayout(m_central_widget_place_holder);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);
            layout->addWidget(m_central_widget);
        }

        void RZEMainWindowCentralWidget::setTitleBarMenu(QMenuBar* menuBar)
        {

        }

    }    // namespace Editor
}    // namespace Razix