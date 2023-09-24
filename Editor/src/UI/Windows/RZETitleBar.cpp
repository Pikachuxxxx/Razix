// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZETitleBar.h"

#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

#define GET_X_LPARAM(lp) ((int) (short) LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int) (short) HIWORD(lp))

namespace Razix {
    namespace Editor {
        RZETitleBar::RZETitleBar(QWidget* child, QWidget* parent)
            : QFrame(parent)
        {
            ui.setupUi(this);
            setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
            setAttribute(Qt::WA_TranslucentBackground);
            QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
            effect->setBlurRadius(9.0);
            effect->setColor(QColor(0, 0, 0, 160));
            effect->setOffset(4.0);

            // int     side = 10;
            // QRegion maskedRegion(width() / 2 - side / 2, height() / 2 - side / 2, side, side, QRegion::Ellipse);
            // setMask(maskedRegion);

            if (child != nullptr) {
                ui.body->layout()->addWidget(child);

                ui.body->setGraphicsEffect(effect);
                child->setGraphicsEffect(effect);

                m_MainBody = child;
                m_MainBody->installEventFilter(this);
                resize(child->size());
            }

            connect(ui.minimize, SIGNAL(pressed()), this, SLOT(on_minimum_clicked()));
            connect(ui.maximize, SIGNAL(pressed()), this, SLOT(on_maximum_clicked()));
            connect(ui.close, SIGNAL(pressed()), this, SLOT(on_close_clicked()));
        }

        RZETitleBar::~RZETitleBar()
        {
            int subBodies = ui.body->layout()->count();
            if (subBodies > 0) {
                for (int i = 0; i < subBodies; i++) {
                    QWidget* subBody = ui.body->layout()->itemAt(i)->widget();
                    delete subBody;
                }
            }
        }

        void RZETitleBar::on_close_clicked()
        {
            close();
        }

        void RZETitleBar::on_maximum_clicked()
        {
            if (isMaximized()) {
                showNormal();
            } else {
                showMaximized();
            }
        }

        void RZETitleBar::on_minimum_clicked()
        {
            showMinimized();
        }

        void RZETitleBar::mousePressEvent(QMouseEvent* event)
        {
            if (event->buttons() == Qt::LeftButton) {
                QWidget* widget = childAt(event->x(), event->y());
                if (widget == ui.header) {
                    m_Position.setX(event->x());
                    m_Position.setY(event->y());
                }
            }
        }

        void RZETitleBar::mouseMoveEvent(QMouseEvent* event)
        {
            if (event->buttons() == Qt::LeftButton) {
                if (m_Position.x() != 0 || m_Position.y() != 0) {
                    move(event->globalX() - m_Position.x(), event->globalY() - m_Position.y());
                }
            }
        }

        void RZETitleBar::mouseReleaseEvent(QMouseEvent* event)
        {
            m_Position.setX(0);
            m_Position.setY(0);
        }

        void RZETitleBar::mouseDoubleClickEvent(QMouseEvent* event)
        {
            if (event->buttons() == Qt::LeftButton) {
                QWidget* widget = childAt(event->x(), event->y());
                if (widget == ui.header) {
                    isMaximized() ? showNormal() : showMaximized();
                }
            }
        }

        bool RZETitleBar::nativeEvent(const QByteArray& eventType, void* message, long* result)
        {
            Q_UNUSED(eventType)
            MSG* param = static_cast<MSG*>(message);
            switch (param->message) {
                case WM_NCHITTEST: {
                    int nX = GET_X_LPARAM(param->lParam) - geometry().x();
                    int nY = GET_Y_LPARAM(param->lParam) - geometry().y();

                    if (childAt(nX, nY) != ui.header && childAt(nX, nY) != ui.body) {
                        return QWidget::nativeEvent(eventType, message, result);
                    }

                    *result = HTCAPTION;

                    if ((nX > 0) && (nX < borderSize)) {
                        *result = HTLEFT;
                    }

                    if ((nX > width() - borderSize) && (nX < width())) {
                        *result = HTRIGHT;
                    }

                    if ((nY > 0) && (nY < borderSize)) {
                        *result = HTTOP;
                    }

                    if ((nY > height() - borderSize) && (nY < height())) {
                        *result = HTBOTTOM;
                    }

                    if ((nX > 0) && (nX < borderSize) && (nY > 0) && (nY < borderSize)) {
                        *result = HTTOPLEFT;
                    }

                    if ((nX > width() - borderSize) && (nX < width()) && (nY > 0) && (nY < borderSize)) {
                        *result = HTTOPRIGHT;
                    }

                    if ((nX > 0) && (nX < borderSize) && (nY > height() - borderSize) && (nY < height())) {
                        *result = HTBOTTOMLEFT;
                    }

                    if ((nX > width() - borderSize) && (nX < width()) && (nY > height() - borderSize) && (nY < height())) {
                        *result = HTBOTTOMRIGHT;
                    }

                    return true;
                }
            }

            return QWidget::nativeEvent(eventType, message, result);
        }

        bool RZETitleBar::eventFilter(QObject* obj, QEvent* event)
        {
            if (obj == m_MainBody) {
                if (event->type() == QEvent::HideToParent) {
                    hide();
                    return true;
                }
                if (event->type() == QEvent::ShowToParent) {
                    show();
                    return true;
                }
                return QObject::eventFilter(obj, event);
            } else {
                return QFrame::eventFilter(obj, event);
            }
            return false;
        }

    }    // namespace Editor
}    // namespace Razix