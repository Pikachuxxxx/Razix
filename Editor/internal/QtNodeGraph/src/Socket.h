#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <QPointF>

class Node;
class NodeEdge;
class GraphicsSocket;

enum SocketPos
{
    LEFT_TOP,
    LEFT_BOTTOM,
    LEFT_CENTER,    // Support this
    RIGHT_TOP,
    RIGHT_BOTTOM,
    RIGHT_CENTER    // Support this
};

enum SocketType
{
    SOCKET_INPUT,
    SOCKET_OUTPUT
};

enum SocketStyle
{
    CIRCLE,
    SQUARE,
    PIN,
    HEADSHOT
};

class Socket
{
public:
    Socket(Node* node, SocketType type, uint32_t index = 0, SocketStyle style = CIRCLE, SocketPos position = LEFT_TOP, bool multiEdges = true, std::string colorHex = "#FFFF7700", const std::string& name = "socket_name");
    ~Socket() {}

    QPointF                   getPos();
    inline SocketPos          getSocketPos() { return position; }
    inline Node*              getNode() { return node; }
    inline const std::string& getSocketName() { return m_Name; }
    inline const void         setSocketName(const std::string& name) { m_Name = name; }

    inline bool                   hasEdges() { return edges.size() > 0 ? true : false; }
    inline void                   addEdge(NodeEdge* edge) { edges.push_back(edge); }
    void                          remove();
    void                          removeEdges();
    void                          removeEdge(NodeEdge* edge);
    inline std::vector<NodeEdge*> getEdges() { return edges; }
    inline bool                   supportsMultiEdges() { return m_SupportsMultipleEdges; }
    SocketType                    getType() { return type; }
    SocketStyle                   getStyle() { return style; }

private:
    Node*                  node  = nullptr;
    std::vector<NodeEdge*> edges = {};    // Denotes the edges to which this socket is connected
    uint32_t               index;
    SocketPos              position;
    SocketType             type;
    SocketStyle            style;
    GraphicsSocket*        grSocket = nullptr;
    std::string            m_Name;
    bool                   m_SupportsMultipleEdges = true;
};
