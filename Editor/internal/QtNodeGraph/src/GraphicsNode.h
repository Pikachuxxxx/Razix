#pragma once

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QPen>

class Node;
class MoveNodeCommand;

#define SOCKET_SPACING 32

class IGraphicsNode : public QGraphicsItem
{
public:
    IGraphicsNode(Node* node);
    virtual ~IGraphicsNode() {}

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override
    {
        hovered = true;
        update();
    }

    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override
    {
        hovered = false;
        update();
    }

    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }
    uint32_t getTitleHeight() const { return titleHeight; }
    uint32_t getEdgeSize() const { return edge_size; }
    uint32_t getPadding() const { return padding; }

    inline Node* getNode() { return node; }

    QGraphicsTextItem* getTextItem() { return titleItem; }

    QBrush getTitleBrush() { return titleBrush; }

protected:
    Node*              node      = nullptr;
    QGraphicsTextItem* titleItem = nullptr;
    QColor             titleColor;
    uint32_t           width       = 180;
    uint32_t           height      = 240;
    uint32_t           edge_size   = 8;
    uint32_t           titleHeight = 24;
    uint32_t           padding     = 6;
    QPen               penDefault;
    QPen               penSelected;
    QPen               penHovered;
    QBrush             titleBrush;
    QBrush             bgBrush;
    QPointF            m_NodeOldPos;
    bool               hovered = false;
};

class GraphicsNode : public IGraphicsNode
{
public:
    GraphicsNode(Node* node);
    ~GraphicsNode() { prepareGeometryChange(); }

    QRectF boundingRect() const override { return QRectF(0, 0, width, height).normalized(); }
    void   paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};
