#include "GraphicsCutLine.h"

#include <QPainter>

GraphicsCutLine::GraphicsCutLine(QGraphicsItem* parent /*= nullptr*/)
    : QGraphicsItem(parent)
{
    pen = QPen(Qt::white);
    pen.setWidth(1.0f);
    pen.setDashPattern({ 5, 5 });

    setZValue(12);
}

void GraphicsCutLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /*= nullptr*/)
{
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(pen);

    auto poly = QPolygon(linePoints);
    painter->drawPolygon(poly);
}

QPainterPath GraphicsCutLine::shape() const
{
    QPainterPath path;
    if (linePoints.size() > 1) {
        path = QPainterPath(linePoints[0]);
        for (auto line : linePoints)
            path.lineTo(line);
    }
    else {
        path = QPainterPath(QPointF(0, 0));
        path.lineTo(QPointF(1, 1));
    }

    return path;
}
