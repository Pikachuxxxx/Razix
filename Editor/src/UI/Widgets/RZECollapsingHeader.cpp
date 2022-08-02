#include "RZECollapsingHeader.h"

#include <QIcon>
#include <QLabel>

namespace Razix {
    namespace Editor {
        RZECollapsingHeader::RZECollapsingHeader(QString& headerTitle, QWidget* childWidget, QIcon* icon, QWidget* parent /* = nullptr*/)
            : QFrame(parent), m_ChildWidget(childWidget)
        {
            m_BoxVLayout = new QVBoxLayout(this);

            m_Header = new RZEHeaderFrame(headerTitle, icon, true, this);

            m_BoxVLayout->setSpacing(4);
            m_BoxVLayout->setMargin(0);

            m_BoxVLayout->addWidget(m_Header);
            m_BoxVLayout->addWidget(m_ChildWidget);

            connect(m_Header, SIGNAL(clicked()), this, SLOT(toggleCollapse()));
        }

        RZECollapsingHeader::~RZECollapsingHeader()
        {}

        RZEHeaderFrame::RZEHeaderFrame(QString& headerTitle, QIcon* icon /*= nullptr*/, bool isCollapsed /*= true*/, QWidget* parent /*= nullptr*/)
            : QFrame(parent)
        {
            this->setMaximumHeight(24);
            //this->move(QPoint(24, 0));
            this->setStyleSheet("border:1px solid rgb(81, 81, 81); background-color: rgb(141, 141, 141); ");

            auto Hlayout = new QHBoxLayout(this);
            Hlayout->setContentsMargins(0, 0, 0, 0);
            Hlayout->setSpacing(0);

            arrow = new Arrow(this);
            Hlayout->addWidget(arrow);

            auto pixmap     = icon->pixmap(20, 20);

            if (icon != nullptr) {
                auto iconLbl = new QLabel();
                iconLbl->setMaximumHeight(24);
                iconLbl->setMaximumWidth(24);
                //iconLbl->move(QPoint(24, 0));
                iconLbl->setStyleSheet("border:0px");
                iconLbl->setPixmap(pixmap);

                Hlayout->addWidget(iconLbl);
            }

            auto title = new QLabel(headerTitle);
            title->setMinimumHeight(24);
            //title->move(QPoint(24, 0));
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
