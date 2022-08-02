#include "RZECollapsingHeader.h"

#include <QIcon>
#include <QLabel>

namespace Razix {
    namespace Editor {
        RZECollapsingHeader::RZECollapsingHeader(QString& headerTitle, QWidget* parent /* = nullptr*/)
            : QFrame(parent)
        {
            m_BoxVLayout = new QVBoxLayout(this);

            m_Header = new RZEHeaderFrame(headerTitle, true, this);
            m_BoxVLayout->addWidget(m_Header);

            connect(m_Header, SIGNAL(clicked()), this, SLOT(toggleCollapse()));
        }

        RZECollapsingHeader::~RZECollapsingHeader()
        {}

        RZEHeaderFrame::RZEHeaderFrame(QString& headerTitle, bool isCollapsed /*= true*/, QWidget* parent /*= nullptr*/)
            : QFrame(parent)
        {
            this->setMaximumHeight(24);
            this->move(QPoint(24, 0));
            this->setStyleSheet("border:1px solid rgb(41, 41, 41); ");

            auto Hlayout = new QHBoxLayout(this);
            Hlayout->setContentsMargins(0, 0, 0, 0);
            Hlayout->setSpacing(0);

            arrow = new Arrow(this);
            Hlayout->addWidget(arrow);

            auto IconWidget = new QIcon(":/rzeditor/RazixLogo64.png");
            auto pixmap     = IconWidget->pixmap(24, 24);

            auto icon = new QLabel();
            icon->setMaximumHeight(24);
            icon->setMaximumWidth(24);
            icon->move(QPoint(24, 0));
            icon->setStyleSheet("border:0px");
            icon->setPixmap(pixmap);

            Hlayout->addWidget(icon);

            auto title = new QLabel(headerTitle);
            title->setMinimumHeight(24);
            title->move(QPoint(24, 0));
            title->setStyleSheet("border:0px");

            Hlayout->addWidget(title);
        }

        Arrow::Arrow(bool collapsed, QWidget* parent /*= nullptr*/)
            : QFrame(parent)
        {
            this->setMaximumSize(24, 24);

            arrowH = new QPolygonF({QPointF(7.0, 8.0), QPointF(17.0, 8.0), QPointF(12.0, 13.0)});
            arrowV = new QPolygonF({QPointF(8.0, 7.0), QPointF(13.0, 12.0), QPointF(8.0, 17.0)});
            arrow  = nullptr;
            setArrow(int(collapsed));
        }

        void Arrow::setArrow(bool isCollapded)
        {
            if (isCollapded)
                arrow = arrowH;
            else
                arrow = arrowV;

            this->update();
        }

    }    // namespace Editor
}    // namespace Razix
