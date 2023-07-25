// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEMemoryPoolView.h"

#include <QPainter>
#include <QPen>

namespace Razix {
    namespace Editor {

        RZEMemoryPoolView::RZEMemoryPoolView()
        {
        }

        RZEMemoryPoolView::~RZEMemoryPoolView()
        {
        }

        void RZEMemoryPoolView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr*/)
        {
            // Memory Block
            auto BlockPath = QPainterPath();
            BlockPath.addRect(25, 0, m_Width - 50, m_Height - 25);

            painter->setPen(QPen(QColor("#121212")));

            QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, 50));
            linearGrad.setColorAt(0, QColor("#C9DE96"));
            linearGrad.setColorAt(0.44, QColor("#8AB66B"));
            linearGrad.setColorAt(0.8, QColor("#398235"));
            painter->setBrush(linearGrad);

            painter->drawPath(BlockPath.simplified());

            auto SizeTextPath = QPainterPath();

            QFont typeFont;
            typeFont.setWeight(QFont::Light);
            typeFont.setStyleHint(QFont::Courier, QFont::PreferAntialias);
            typeFont.setPointSize(20);
            typeFont.setBold(true);
            QFontMetrics fm(typeFont);
            painter->setPen(QPen(QColor("#000000")));
            painter->setBrush(QBrush(QColor("#000000")));
            SizeTextPath.addText(m_Width / 2 - fm.width("448 Mib") / 2, fm.height(), typeFont, "448 Mib");

            painter->drawPath(SizeTextPath.simplified());

#if 0
            QFont typeFont;
            typeFont.setWeight(QFont::Light);
            typeFont.setStyleHint(QFont::Times, QFont::PreferAntialias);
            typeFont.setPointSize(10);

            // Text BG Box
            auto TypeTextBoxPath = new QPainterPath();
            QPen rr(QColor("#ddc988"));
            rr.setWidth(2);
            painter->setPen(rr);
            painter->setBrush(QBrush(QColor("#504b35")));
            QFontMetrics fm(typeFont);
            painter->drawRoundedRect(QRect(0, -fm.height() - 25, fm.width("Type : Texture2D") + 10, fm.height() + 10), 5, 5);

            painter->drawPath(TypeTextBoxPath->simplified());

            // Text for Type
            auto TypeTextPath = new QPainterPath();
            painter->setPen(Qt::white);
            painter->setBrush(QColor("#FFFFFF"));
            TypeTextPath->addText(5, -25, typeFont, QString("Type : Texture2D"));

            painter->drawPath(TypeTextPath->simplified());
#endif

            paintBorderRectWithText(painter, QString("Type : Texture2D"), QColor("#504b35"), QColor("#ddc988"), 5, -25);

            paintBorderRectWithText(painter, QString("43"), QColor("#8d1439"), QColor("#e75380"), m_SliderValue, 90);

            // Occupancy Slider
            auto OccupancyBarPath = new QPainterPath;
            OccupancyBarPath->addRect(m_SliderValue, 0, 5, 60);
            painter->setBrush(QColor("#FF11FF"));
            painter->setPen(QColor("#FF11FF"));
            QPolygonF Triangle;
            Triangle.append(QPointF(10., 0));
            Triangle.append(QPointF(0., -10));
            Triangle.append(QPointF(-10., 0));
            Triangle.append(QPointF(10., 0));
            Triangle.translate(m_SliderValue + 2.5, 67.5);
            OccupancyBarPath->addPolygon(Triangle);

            painter->drawPath(OccupancyBarPath->simplified());
        }

        void RZEMemoryPoolView::paintBorderRectWithText(QPainter *painter, QString string, QColor bgColor, QColor borderColor, float x, float y)
        {
            QFont typeFont;
            typeFont.setWeight(QFont::Light);
            typeFont.setStyleHint(QFont::Courier, QFont::PreferAntialias);
            typeFont.setPointSize(10);

            // Text BG Box
            auto TypeTextBoxPath = new QPainterPath();
            QPen rr(borderColor);
            rr.setWidth(2);
            painter->setPen(rr);
            painter->setBrush(QBrush(bgColor));
            QFontMetrics fm(typeFont);
            painter->drawRoundedRect(QRect(x, -fm.height() + y, fm.width(string) + 10, fm.height() + 10), 5, 5);

            painter->drawPath(TypeTextBoxPath->simplified());

            // Text for Type
            auto TypeTextPath = new QPainterPath();
            painter->setPen(Qt::white);
            painter->setBrush(QColor("#FFFFFF"));
            TypeTextPath->addText(x + 5, y, typeFont, string);

            painter->drawPath(TypeTextPath->simplified());
        }

    }    // namespace Editor
}    // namespace Razix
