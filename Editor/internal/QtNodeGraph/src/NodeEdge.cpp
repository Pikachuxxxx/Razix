#include "NodeEdge.h"

#include "NodeScene.h"
#include "Socket.h"
#include "GraphicsEdge.h"

#include <iostream>

NodeEdge::NodeEdge(NodeScene* scene, Socket* startSocket, Socket* endSocket, EdgeType type)
    : m_Scene(scene), startSocket(startSocket), endSocket(endSocket), type(type)
{
    if (type == BEZIER)
        grEdge = new GraphicsEdgeBezier(this);
    else if (type == DIRECT)
        grEdge = new GraphicsEdgeDirect(this);

    scene->addEdge(this);
    scene->getGraphicsScene()->addItem(grEdge);

    if (startSocket)
        startSocket->addEdge(this);

    if (endSocket)
        endSocket->addEdge(this);
}

void NodeEdge::setStartSocket(Socket* socket)
{
    // Remove existing node for the existing socket
    if (startSocket)
        startSocket->removeEdge(this);

    startSocket = socket;
    startSocket->addEdge(this);
    //grEdge = socket->getConnectedEdge()->grEdge;
    if (grEdge)
        grEdge->updatePath();

}
void NodeEdge::setEndSocket(Socket* socket)
{
    // Remove existing node for the existing socket
    if (endSocket)
        endSocket->removeEdge(this);

    endSocket = socket;
    endSocket->addEdge(this);
    //grEdge = socket->getConnectedEdge()->grEdge;
    if (grEdge)
        grEdge->updatePath();
}

void NodeEdge::removeFromSockets()
{
    if (startSocket)
        startSocket->removeEdge(this);
    if (endSocket)
        endSocket->removeEdge(this);
    startSocket = nullptr;
    endSocket = nullptr;
}

void NodeEdge::remove()
{
    //grEdge->~GraphicsEdge();
    //m_Scene->getGraphicsScene()->removeItem(grEdge);

    
    delete static_cast<QGraphicsItem*>(grEdge);
    grEdge = nullptr;
    removeFromSockets();
    m_Scene->removeEdge(this);
}

void NodeEdge::add()
{
    if (type == BEZIER)
        grEdge = new GraphicsEdgeBezier(this);
    else if (type == DIRECT)
        grEdge = new GraphicsEdgeDirect(this);

    m_Scene->addEdge(this);
    m_Scene->getGraphicsScene()->addItem(grEdge);

    if (startSocket)
        startSocket->addEdge(this);

    if (endSocket)
        endSocket->addEdge(this);
}

void NodeEdge::select()
{
    grEdge->setSelected(true);
}
