#pragma once

#include <QBoxLayout>
#include <QFrame>
#include <QPainter>

#include <iostream>

namespace Razix {
    namespace Editor {

        class Arrow : public QFrame
        {
            Q_OBJECT
        public:
            Arrow(bool collapsed, QWidget* parent = nullptr);
            ~Arrow() {}

            void setArrow(bool isCollapded);

            void paintEvent(QPaintEvent* event)
            {
                QPainter painter(this);
                //painter.begin(this);
                painter.setBrush(QColor(192, 192, 192));
                painter.setPen(QColor(64, 64, 64));
                painter.drawPolygon(*arrow);
                //painter.end();
            }

        private:
            QPolygonF* arrowH;
            QPolygonF* arrowV;
            QPolygonF* arrow;
        };

        class RZEHeaderFrame : public QFrame
        {
            Q_OBJECT
        public:
            RZEHeaderFrame(QString& headerTitle, QIcon* icon = nullptr, bool isCollapsed = true, QWidget* parent = nullptr);
            ~RZEHeaderFrame() {}
            Arrow* arrow;

        signals:
            void clicked();

        private:
            void mousePressEvent(QMouseEvent* event)
            {
                emit clicked();
                QFrame::mousePressEvent(event);
            }
        };

        // TODO: Add either delete/remove or more pop-up options button 
        class RZECollapsingHeader : public QFrame
        {
            Q_OBJECT

        public:
            RZECollapsingHeader(QString& headerTitle, QWidget* childWidget, QIcon* icon = nullptr, QWidget* parent = nullptr);
            ~RZECollapsingHeader();

        private:
            bool            m_IsCollapsed = true;
            QVBoxLayout*    m_BoxVLayout;
            RZEHeaderFrame* m_Header;
            QWidget*        m_ChildWidget;

        public slots:
            void toggleCollapse()
            {
                m_Header->arrow->setArrow(m_IsCollapsed);
                m_IsCollapsed = !m_IsCollapsed;

                if (!m_IsCollapsed)
                    m_ChildWidget->show();
                else
                    m_ChildWidget->hide();
            }
        };
    }    // namespace Editor
}    // namespace Razix
