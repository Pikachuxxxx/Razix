#include "Socket.h"

#include "GraphicsNode.h"
#include "GraphicsSocket.h"
#include "Node.h"
#include "NodeEdge.h"

#include <iostream>

Socket::Socket(Node* node, SocketType type, uint32_t index, SocketStyle style, SocketPos position, bool multiEdges, std::string colorHex, const std::string& name)
    : node(node), index(index), position(position), m_SupportsMultipleEdges(multiEdges), m_Name(name), type(type), style(style)
{
    grSocket = new GraphicsSocket(this, colorHex);
    grSocket->setPos(node->getSocketPosition(index, position));
}

QPointF Socket::getPos()
{
    // return grSocket->pos();
    auto Pos = node->getSocketPosition(index, position);
    if (style == HEADSHOT) {
        if (position == RIGHT_BOTTOM || position == RIGHT_TOP)
            Pos += QPoint(12, 6);
        else if (position == LEFT_BOTTOM || position == LEFT_TOP)
            Pos -= QPoint(12, -6);
    }
    return Pos;
}

void Socket::remove()
{
    removeEdges();
    delete grSocket;
    grSocket = nullptr;
}

void Socket::removeEdges()
{
    for (auto edge: edges)
        edge->remove();
}

void Socket::removeEdge(NodeEdge* edge)
{
    if (std::find(edges.begin(), edges.end(), edge) != edges.end())
        edges.erase(std::remove(edges.begin(), edges.end(), edge), edges.end());
}
