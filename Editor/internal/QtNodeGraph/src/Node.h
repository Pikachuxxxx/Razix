#pragma once

#include <string>

#include <QGraphicsItem>
#include <QUndoCommand>

#include "Socket.h"

class NodeScene;
class IGraphicsNode;
class GraphicsNode;
class NodeContentWidget;

// TODO: Remove NodeScene from the Node class and use utility methods in NodeScene class to add/create nodes
class Node
{
public:
    // TODO: Use a array for input and output counts that take the node colors/types in future
    Node(NodeScene* scene, std::string nodeName, SocketStyle style, uint32_t inputsCount = 0, uint32_t outputsCount = 0, bool multiEdgesInput = false, bool multiEdgesOutput = true);
    Node(NodeScene* scene, std::string nodeName, SocketStyle style, std::vector<std::string> inputsCount, std::vector<std::string> outputsCount, bool multiEdgesInput = false, bool multiEdgesOutput = true);
    virtual ~Node() {}

    void remove();
    void add();

    void update();

    void addInputSocket(const std::string& name = "input_socket", SocketStyle style = CIRCLE, SocketPos pos = LEFT_TOP, const std::string& hexColor = "#FFFF7700");
    void removeInputSocket(uint32_t idx);
    void addOutputSocket(const std::string& name = "input_socket", SocketStyle style = CIRCLE, SocketPos pos = RIGHT_TOP, const std::string& hexColor = "#FF00A5FF");
    void removeOutputSocket(uint32_t idx);

    QPointF                     getSocketPosition(uint32_t index, SocketPos pos);
    inline QPointF              getPos();
    void                        setPos(uint32_t x, uint32_t y);
    inline NodeScene*           getScene() { return scene; }
    inline const std::string&   getTitle() const { return title; }
    void                        setTitle(const std::string& title);
    inline NodeContentWidget*   getContent() { return nodeContent; }
    inline void                 setContent(NodeContentWidget* widget) { nodeContent = widget; }
    inline IGraphicsNode*       getGraphicsNode() { return graphicsNode; }
    void                        setGraphicsNode(IGraphicsNode* grNode);
    inline Socket*              getInputSocket(uint32_t idx) { return inputs[idx]; }
    inline Socket*              getOutputSocket(uint32_t idx) { return outputs[idx]; }
    inline std::vector<Socket*> getInputSockets() { return inputs; }
    inline std::vector<Socket*> getOutputSockets() { return outputs; }

private:
    NodeScene*           scene = nullptr;
    std::string          title;
    IGraphicsNode*       graphicsNode = nullptr;
    NodeContentWidget*   nodeContent;
    std::vector<Socket*> inputs;
    std::vector<Socket*> outputs;
    uint32_t             socketSpacing      = 24;
    bool                 m_MultiEdgesInput  = false;
    bool                 m_MultiEdgesOutput = true;
};

// Commands for Undo and Redo
// Add, Remove and Move variations

class AddNodeCommand : public QUndoCommand
{
public:
    AddNodeCommand(Node* node, QGraphicsScene* scene);

    void undo() override;
    void redo() override;

private:
    Node*           mNode          = nullptr;
    QGraphicsScene* mGraphicsScene = nullptr;
    QPointF         mInitialPosition;
};

class RemoveNodeCommand : public QUndoCommand
{
public:
    RemoveNodeCommand(QGraphicsScene* scene);

    void undo() override;
    void redo() override;

private:
    Node*           mNode          = nullptr;
    QGraphicsScene* mGraphicsScene = nullptr;
    QPointF         mInitialPosition;
};

class MoveNodeCommand : public QUndoCommand
{
public:
    MoveNodeCommand(Node* node, QPointF oldPos, QGraphicsScene* scene);

    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand* other) override;

private:
    Node*           mNode          = nullptr;
    QGraphicsScene* mGraphicsScene = nullptr;
    QPointF         mOldPosition;
    QPointF         mNewPosition;
};