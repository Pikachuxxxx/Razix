#pragma once

#include <QGraphicsItem>
#include <QPen>

class GraphicsCutLine : public QGraphicsItem
{
public:
    GraphicsCutLine(QGraphicsItem* parent = nullptr);
    ~GraphicsCutLine() {}

    QRectF boundingRect() const override { return shape().boundingRect(); }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    QPainterPath shape() const override;

    inline void clearLinePoints() { linePoints.clear(); }
    inline QVector<QPoint> getLinePoints() { return linePoints; }
    inline void addPoint(QPoint point) { linePoints.push_back(point); }

private:
    QPen pen;
    QVector<QPoint> linePoints;
};

