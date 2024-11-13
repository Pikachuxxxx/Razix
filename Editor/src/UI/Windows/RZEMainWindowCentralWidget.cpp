// clang-format off
#include "rzepch.h"
// clang-format on

#include "RZEMainWindowCentralWidget.h"

#include "Razix/Core/App/RZApplication.h"

#include "Razix/Utilities/RZStringUtilities.h"

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

            m_Titlebar = new RZETitleBar(gw, this);
            m_Titlebar->installEventFilter(this);

            m_gw->setTitleBarHeight(m_Titlebar->height());

            // Frame to hold the MainWindow.ui I suppose
            m_frame = new QFrame(this);
            m_frame->setObjectName("GoodFrame");
            //m_frame->setAttribute(Qt::WA_TransparentForMouseEvents);

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
            menuBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            menuBar->setStyleSheet("background-color:#FF242424");
            m_Titlebar->setMenuBar(menuBar);
        }

        void RZEMainWindowCentralWidget::updateWindow()
        {
#ifdef QGOODWINDOW
            QTimer::singleShot(0, this, &RZEMainWindowCentralWidget::updateWindowLater);
#endif
        }

        void RZEMainWindowCentralWidget::updateWindowLater()
        {
            QRegion     mask;
            QWidgetList list;

            list.append(m_Titlebar->findChildren<QWidget*>());

            for (QWidget* widget: list) {
                auto widgetName   = widget->objectName().toStdString();
                bool isMenuWidget = Razix::Utilities::StartsWith(widgetName, "menu");

                if (!widget->testAttribute(Qt::WA_TransparentForMouseEvents) && (widgetName == "closeBtn" || widgetName == "minimizeBtn" || widgetName == "maximizeBtn" || isMenuWidget)) {
                    if (!widget->mask().isNull()) {
                        mask += widget->mask().translated(m_Titlebar->pos());
                    } else {
                        QRect geom = widget->geometry();

                        if (geom.width() > m_Titlebar->width())
                            geom.setWidth(m_Titlebar->width());

                        mask += geom.translated(m_Titlebar->pos());
                    }
                }
            }

            m_gw->setTitleBarMask(mask);

            return;
        }

        bool RZEMainWindowCentralWidget::eventFilter(QObject* watched, QEvent* event)
        {
#ifdef QGOODWINDOW
            if (watched == m_gw) {
                switch (event->type()) {
                    case QEvent::Show:
                    case QEvent::Resize:
                    case QEvent::WindowStateChange:
                    case QEvent::WindowActivate:
                    case QEvent::WindowDeactivate: {
                        updateWindow();
                        break;
                    }
                    default:
                        break;
                }
            } else if (watched == m_Titlebar) {
                switch (event->type()) {
                    case QEvent::Show: {
                        updateWindow();
                        break;
                    }
                    default:
                        break;
                }
            }
#endif
            return QWidget::eventFilter(watched, event);
        }

        bool RZEMainWindowCentralWidget::event(QEvent* event)
        {
#ifdef QGOODWINDOW
    #ifdef Q_OS_LINUX
            switch (event->type()) {
                case QEvent::Show:
                case QEvent::Resize: {
                    QTimer::singleShot(0, this, [=] {
                        if (!m_gw)
                            return;

                        if (!m_central_widget_place_holder)
                            return;

                        QRegion mask;

                        if (m_gw->isVisible() && m_gw->windowState().testFlag(Qt::WindowNoState)) {
                            const int radius = 8;

                            QBitmap bmp(m_central_widget_place_holder->size());
                            bmp.clear();

                            QPainter painter;
                            painter.begin(&bmp);
                            painter.setRenderHints(QPainter::Antialiasing);
                            painter.setPen(Qt::color1);
                            painter.setBrush(Qt::color1);
                            painter.drawRoundedRect(m_central_widget_place_holder->rect().adjusted(1, 1, -1, -1),
                                radius,
                                radius,
                                Qt::AbsoluteSize);
                            painter.end();

                            mask = bmp;
                        }

                        m_central_widget_place_holder->setMask(mask);
                    });

                    break;
                }
                default:
                    break;
            }
    #endif
#endif

            return QWidget::event(event);
        }

    }    // namespace Editor
}    // namespace Razix