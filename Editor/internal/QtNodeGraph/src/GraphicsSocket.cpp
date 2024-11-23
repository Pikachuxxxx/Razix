#include "GraphicsSocket.h"

#include "GraphicsNode.h"
#include "Node.h"
#include "Socket.h"

GraphicsSocket::GraphicsSocket(Socket* socket, std::string colorHex)
    : QGraphicsItem(socket->getNode()->getGraphicsNode()), m_Socket(socket)
{
    // this->setPos(node->getScene()->getOrigin().x(), node->getScene()->getOrigin().y());
    this->setFlags(QGraphicsItem::ItemIsSelectable);
    bgColor      = QColor(colorHex.c_str());
    outlineColor = QColor("#FF000000");

    pen = QPen(outlineColor);
    pen.setWidthF(outlineWidth);

    penHovered = QPen("#FF37A6FF");
    penHovered.setWidthF(outlineWidth * 2.0f);

    brush = QBrush(bgColor);

    this->setZValue(5);
}

QRectF GraphicsSocket::boundingRect() const
{
    if (m_Socket->getStyle() == CIRCLE || m_Socket->getStyle() == SQUARE)
        return QRectF(-(radius + outlineWidth), -(radius + outlineWidth), 2 * (radius + outlineWidth), 2 * (radius + outlineWidth));
    else if (m_Socket->getStyle() == PIN) {
        QPolygonF arrowV;
        auto      spos = m_Socket->getSocketPos();
        if (spos == LEFT_TOP || spos == LEFT_BOTTOM) {
            arrowV = QPolygonF({QPointF(0.0 + offset, 0.0 - radius),
                QPointF(0.0 + offset, 24.0 - radius),
                QPointF(8.0 + offset, 24.0 - radius),
                QPointF(16.0 + offset, 12.0 - radius),
                QPointF(16.0 + offset, 12.0 - radius),
                QPointF(8.0 + offset, 0.0 - radius),
                QPointF(0.0 + offset, 0.0 - radius)});
        } else {
            arrowV = QPolygonF({QPointF(0.0 - offset * 2, 0.0 - radius),
                QPointF(0.0 - offset * 2, 24.0 - radius),
                QPointF(8.0 - offset * 2, 24.0 - radius),
                QPointF(16.0 - offset * 2, 12.0 - radius),
                QPointF(16.0 - offset * 2, 12.0 - radius),
                QPointF(8.0 - offset * 2, 0.0 - radius),
                QPointF(0.0 - offset * 2, 0.0 - radius)});
        }

        QPainterPath path;
        path.addPolygon(arrowV);
        return path.boundingRect();
    } else if (m_Socket->getStyle() == HEADSHOT) {
        QPolygonF arrowV;
        auto      spos = m_Socket->getSocketPos();
        if (spos == LEFT_TOP || spos == LEFT_BOTTOM) {
            arrowV = QPolygonF({QPointF(0.0, 0.0 - radius),
                QPointF(0.0, 24.0 - radius),
                QPointF(-16.0, 12.0 - radius),
                QPointF(0.0, 0.0 - radius)});
        } else {
            arrowV = QPolygonF({QPointF(0.0, 0.0 - radius),
                QPointF(0.0, 24.0 - radius),
                QPointF(16.0, 12.0 - radius),
                QPointF(0.0, 0.0 - radius)});
        }
        QPainterPath path;
        path.addPolygon(arrowV);
        return path.boundingRect();
    }

    return QRectF();
}

void GraphicsSocket::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // Painting circle
    painter->setBrush(brush);

    if (m_Socket->getStyle() == CIRCLE) {
        if (hover) {
            painter->setPen(penHovered);
            painter->drawEllipse(-radius, -radius, 2 * radius, 2 * radius);
        }
        painter->setPen(pen);
        painter->drawEllipse(-radius, -radius, 2 * radius, 2 * radius);
    } else if (m_Socket->getStyle() == SQUARE) {
        painter->setPen(pen);
        painter->drawRect(-radius, -radius, 2 * radius, 2 * radius);
    } else if (m_Socket->getStyle() == PIN) {
        // If it has edged draw filled white pin
        if (m_Socket->hasEdges()) {
            auto whitePen = QPen(QColor("#FFFFFF"));
            whitePen.setWidthF(3.0f);
            painter->setPen(whitePen);
            painter->setBrush(QBrush(QColor("#FFFFFF")));
        } else {
            auto whitePen = QPen(QColor("#FFFFFF"));
            whitePen.setWidthF(3.0f);
            painter->setPen(whitePen);
            painter->setBrush(Qt::NoBrush);
        }

        QPolygonF arrowV;
        auto      spos = m_Socket->getSocketPos();
        if (spos == LEFT_TOP || spos == LEFT_BOTTOM) {
            arrowV = QPolygonF({QPointF(0.0 + offset, 0.0 - radius),
                QPointF(0.0 + offset, 24.0 - radius),
                QPointF(8.0 + offset, 24.0 - radius),
                QPointF(16.0 + offset, 12.0 - radius),
                QPointF(16.0 + offset, 12.0 - radius),
                QPointF(8.0 + offset, 0.0 - radius),
                QPointF(0.0 + offset, 0.0 - radius)});
        } else {
            arrowV = QPolygonF({QPointF(0.0 - offset * 2, 0.0 - radius),
                QPointF(0.0 - offset * 2, 24.0 - radius),
                QPointF(8.0 - offset * 2, 24.0 - radius),
                QPointF(16.0 - offset * 2, 12.0 - radius),
                QPointF(16.0 - offset * 2, 12.0 - radius),
                QPointF(8.0 - offset * 2, 0.0 - radius),
                QPointF(0.0 - offset * 2, 0.0 - radius)});
        }

        painter->drawPolygon(arrowV);
    } else if (m_Socket->getStyle() == HEADSHOT) {
        /**
         * In head shot style the pin originated from the border of the Node outline and gives a small arrow like shape
         * 
         */

        QPolygonF arrowV;
        auto      spos = m_Socket->getSocketPos();
        if (spos == LEFT_TOP || spos == LEFT_BOTTOM) {
            arrowV = QPolygonF({QPointF(0.0, 0.0 - radius),
                QPointF(0.0, 24.0 - radius),
                QPointF(-16.0, 12.0 - radius),
                QPointF(0.0, 0.0 - radius)});
        } else {
            arrowV = QPolygonF({QPointF(0.0, 0.0 - radius),
                QPointF(0.0, 24.0 - radius),
                QPointF(16.0, 12.0 - radius),
                QPointF(0.0, 0.0 - radius)});
        }

        painter->setPen(Qt::NoPen);
        painter->setBrush(m_Socket->getNode()->getGraphicsNode()->getTitleBrush());

        painter->drawPolygon(arrowV);
    }

#if 1
    // Paint the name
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(QColor("#FFFFFF")));
    // Calculate this based on the type of the socket and also adjust the width of the node
    QFontMetrics fm   = painter->fontMetrics();
    auto         spos = m_Socket->getSocketPos();
    if (spos == LEFT_TOP || spos == LEFT_BOTTOM)
        painter->drawText(radius, radius, m_Socket->getSocketName().c_str());
    else
        painter->drawText(-radius - fm.horizontalAdvance(m_Socket->getSocketName().c_str()), radius, m_Socket->getSocketName().c_str());
#endif
}
