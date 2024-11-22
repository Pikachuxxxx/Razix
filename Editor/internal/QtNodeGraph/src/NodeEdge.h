#pragma once

#include <QList>
#include <QGraphicsItem>

class NodeScene;
class Socket;
class GraphicsEdge;

enum EdgeType
{
    BEZIER,
    DIRECT
};

class NodeEdge
{
public:
    NodeEdge(NodeScene* scene, Socket* startSocket, Socket* endSocket, EdgeType type = BEZIER);
    ~NodeEdge() {}

    void updatePositions();

    Socket* getStartSocket() const { return startSocket; }
    void setStartSocket(Socket* socket);
    Socket* getEndSocket() const { return endSocket; }
    void setEndSocket(Socket* socket);

    void removeFromSockets();
    void remove();
    void add();
    void select();

    inline GraphicsEdge* getGraphicsEdge() { return grEdge; }
    inline NodeScene* getScene() { return m_Scene; }
private:
    NodeScene* m_Scene = nullptr;
    GraphicsEdge* grEdge = nullptr;
    Socket* startSocket = nullptr;
    Socket* endSocket = nullptr;
    EdgeType type;
};
