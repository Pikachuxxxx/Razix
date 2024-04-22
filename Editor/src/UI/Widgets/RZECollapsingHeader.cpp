// clang-format off
#include "rzepch.h"
// clang-format on

#include "RZECollapsingHeader.h"

#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QLabel>

namespace Razix {
    namespace Editor {
        RZECollapsingHeader::RZECollapsingHeader(QString headerTitle, QWidget* childWidget, QIcon* icon, bool enableUtilButton /*= true*/, QWidget* parent /* = nullptr*/)
            : QFrame(parent), m_ChildWidget(childWidget)
        {
            m_BoxVLayout = new QVBoxLayout(this);

            m_Header = new RZEHeaderFrame(headerTitle, icon, true, enableUtilButton, this);

            m_BoxVLayout->setSpacing(4);
            m_BoxVLayout->setMargin(0);

            m_BoxVLayout->addWidget(m_Header);
            m_BoxVLayout->addWidget(m_ChildWidget);

            connect(m_Header, SIGNAL(clicked()), this, SLOT(toggleCollapse()));

            if (enableUtilButton) {
                connect(m_Header->m_UtilButton, SIGNAL(pressed()), this, SIGNAL(utilButtonClicked()));
            }

            // Start in collapsed
            toggleCollapse();

            QGraphicsDropShadowEffect* DropShadowEffect = new QGraphicsDropShadowEffect();
            DropShadowEffect->setBlurRadius(20);
            DropShadowEffect->setOffset(1, 2);
            DropShadowEffect->setColor(Qt::black);
            this->setGraphicsEffect(DropShadowEffect);
        }

        RZECollapsingHeader::~RZECollapsingHeader()
        {
        }

        RZEHeaderFrame::RZEHeaderFrame(QString& headerTitle, QIcon* icon /*= nullptr*/, bool isCollapsed /*= true*/, bool enableUtilButton /*= true*/, QWidget* parent /*= nullptr*/)
            : QFrame(parent)
        {
            this->setMaximumHeight(30);
            this->setStyleSheet("border:0px solid rgb(81, 81, 81); background-color: rgb(31, 31, 31); ");

            auto Hlayout = new QHBoxLayout(this);
            Hlayout->setContentsMargins(0, 0, 0, 0);
            Hlayout->setSpacing(0);

            arrow = new Arrow(this);
            Hlayout->addWidget(arrow);

            if (icon != nullptr) {
                auto iconLbl = new QLabel();
                auto pixmap  = icon->pixmap(30, 30);
                iconLbl->setMaximumHeight(30);
                iconLbl->setMaximumWidth(30);
                //iconLbl->move(QPoint(24, 0));
                iconLbl->setStyleSheet("border:0px");
                iconLbl->setPixmap(pixmap);

                Hlayout->addWidget(iconLbl);
            }

            auto title = new QLabel(headerTitle);
            title->setMinimumHeight(30);
            //title->move(QPoint(24, 0));
            title->setStyleSheet("border:0px");

            Hlayout->addWidget(title);

            // Add a + button that can be used to add more components
            if (enableUtilButton) {
                QIcon icon(":/rzeditor/Add_icon.png");
                auto  pixmap = icon.pixmap(30, 30);
                m_UtilButton = new QPushButton;
                m_UtilButton->setFixedSize(30, 30);
                m_UtilButton->setIcon(icon);
                //Hlayout->addStretch(2);
                auto hSpacer = new QSpacerItem(0, 30, QSizePolicy::Expanding, QSizePolicy::Minimum);
                Hlayout->addSpacerItem(hSpacer);
                Hlayout->addWidget(m_UtilButton);
            }
        }

        Arrow::Arrow(bool collapsed, QWidget* parent /*= nullptr*/)
            : QFrame(parent)
        {
            this->setMinimumSize(24, 24);
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
