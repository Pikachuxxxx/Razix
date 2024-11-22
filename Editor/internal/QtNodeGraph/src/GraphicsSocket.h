#pragma once

#include <iostream>

#include <QBrush>
#include <QColor>
#include <QGraphicsItem>
#include <QPainter>
#include <QPen>

class Socket;

// TODO: Implement hovering for all types of socket styles
class GraphicsSocket : public QGraphicsItem
{
public:
    GraphicsSocket(Socket* socket, std::string colorHex = "#FFFF7700");
    ~GraphicsSocket() { prepareGeometryChange(); }

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        std::cout << "Socket Pressed" << std::endl;

        QGraphicsItem::mousePressEvent(event);
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override
    {
        std::cout << "Mouse hovering on Socket" << std::endl;

        QGraphicsItem::mouseMoveEvent(event);
    }

    QRectF boundingRect() const override;
    void   paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    Socket* getSocket() { return m_Socket; }

private:
    Socket* m_Socket     = nullptr;
    int32_t radius       = 6;
    int32_t offset       = 16;
    float   outlineWidth = 1.0f;
    QColor  bgColor;
    QColor  outlineColor;
    QPen    pen;
    QPen    penHovered;
    QBrush  brush;
    bool    hover = false;
};
