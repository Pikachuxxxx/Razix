// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZETitleBar.h"

#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

#include <qgoodwindow.h>

#define GET_X_LPARAM(lp) ((int) (short) LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int) (short) HIWORD(lp))

namespace Razix {
    namespace Editor {
        RZETitleBar::RZETitleBar(QGoodWindow* goodWindow)
            : QFrame(goodWindow), m_GoodWindow(goodWindow)
        {
            ui.setupUi(this);
            //setAttribute(Qt::WA_TranslucentBackground);
            QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
            effect->setBlurRadius(5);
            effect->setOffset(2, 4);
            effect->setColor(Qt::black);

            // TODO: Handle buttons state changed events
            connect(m_GoodWindow, &QGoodWindow::captionButtonStateChanged, this, &RZETitleBar::captionButtonStateChanged);

            // TODO: Add restore button -> which is to get back to old state when maximized
            //connect(ui.restoreBtn, SIGNAL(pressed()), m_GoodWindow, SLOT(showNormal()));

            connect(ui.minimizeBtn, SIGNAL(clicked()), m_GoodWindow, SLOT(showMinimized()));
            connect(ui.maximizeBtn, SIGNAL(clicked()), m_GoodWindow, SLOT(showMaximized()));
            connect(ui.closeBtn, SIGNAL(clicked()), m_GoodWindow, SLOT(close()));

            // TODO: QMainWindow title change and Icon change events support
#if 0 
             connect(m_gw, &QGoodWindow::windowTitleChanged, m_title_bar, [=](const QString &title){
                m_title_bar->setTitle(title);
            });

            connect(m_gw, &QGoodWindow::windowIconChanged, m_title_bar, [=](const QIcon &icon){
                if (!icon.isNull())
                {
                    const int pix_size = 16;
                    m_title_bar->setIcon(icon.pixmap(pix_size, pix_size));
                }
            });
#endif
        }

        RZETitleBar::~RZETitleBar()
        {
        }

        void RZETitleBar::captionButtonStateChanged(const QGoodWindow::CaptionButtonState& state)
        {
            switch (state) {
                case QGoodWindow::CaptionButtonState::MinimizeHoverEnter:
                    break;
                case QGoodWindow::CaptionButtonState::MinimizeHoverLeave:
                    break;
                case QGoodWindow::CaptionButtonState::MinimizePress:
                    break;
                case QGoodWindow::CaptionButtonState::MinimizeRelease:
                    break;
                case QGoodWindow::CaptionButtonState::MinimizeClicked:
                    break;
                case QGoodWindow::CaptionButtonState::MaximizeHoverEnter:
                    break;
                case QGoodWindow::CaptionButtonState::MaximizeHoverLeave:
                    break;
                case QGoodWindow::CaptionButtonState::MaximizePress:
                    break;
                case QGoodWindow::CaptionButtonState::MaximizeRelease:
                    break;
                case QGoodWindow::CaptionButtonState::MaximizeClicked:
                    break;
                case QGoodWindow::CaptionButtonState::CloseHoverEnter:
                    break;
                case QGoodWindow::CaptionButtonState::CloseHoverLeave:
                    break;
                case QGoodWindow::CaptionButtonState::ClosePress:
                    break;
                case QGoodWindow::CaptionButtonState::CloseRelease:
                    break;
                case QGoodWindow::CaptionButtonState::CloseClicked:
                    break;
                default:
                    break;
            }
        }

    }    // namespace Editor
}    // namespace Razix