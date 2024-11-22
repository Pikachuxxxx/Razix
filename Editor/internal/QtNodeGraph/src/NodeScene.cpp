#include "NodeScene.h"

#include "Node.h"

NodeScene::NodeScene()
{
    m_GraphicsScene = new NodeGraphicsScene;
}

void NodeScene::addNode(Node* node)
{
    m_Nodes.push_back(node);
}

void NodeScene::removeNode(Node* node)
{
    if (std::find(m_Nodes.begin(), m_Nodes.end(), node) != m_Nodes.end())
        m_Nodes.erase(std::remove(m_Nodes.begin(), m_Nodes.end(), node), m_Nodes.end());
}

void NodeScene::addEdge(NodeEdge* edge)
{
    m_Edges.push_back(edge);
}

void NodeScene::removeEdge(NodeEdge* edge)
{
    if (std::find(m_Edges.begin(), m_Edges.end(), edge) != m_Edges.end())
        m_Edges.erase(std::remove(m_Edges.begin(), m_Edges.end(), edge), m_Edges.end());
}

Node* NodeScene::getNodeByName(const std::string& name)
{
    for (auto node: m_Nodes) {
        if (node->getTitle() == name)
            return node;
    }
}
