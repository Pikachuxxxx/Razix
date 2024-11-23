#include "Node.h"

#include "GraphicsNode.h"
#include "NodeContentWidget.h"
#include "NodeEdge.h"
#include "NodeScene.h"

#include <QUndoCommand>

Node::Node(NodeScene* scene, std::string nodeName, SocketStyle style, uint32_t inputsCount, uint32_t outputsCount, bool multiEdgesInput , bool multiEdgesOutput)
    : scene(scene), title(nodeName), m_MultiEdgesInput(multiEdgesInput), m_MultiEdgesOutput(multiEdgesOutput)
{
    nodeContent  = new NodeContentWidget;
    graphicsNode = new GraphicsNode(this);

    scene->addNode(this);
    scene->getGraphicsScene()->addItem(graphicsNode);

    for (uint32_t i = 0; i < inputsCount; i++) {
        auto socket = new Socket(this, SOCKET_INPUT, i, style, LEFT_BOTTOM, m_MultiEdgesInput);
        inputs.push_back(socket);
    }

    for (uint32_t i = 0; i < outputsCount; i++) {
        auto socket = new Socket(this, SOCKET_OUTPUT, i, style, RIGHT_TOP, m_MultiEdgesOutput);
        outputs.push_back(socket);
    }
}

Node::Node(NodeScene* scene, std::string nodeName, SocketStyle style, std::vector<std::string> inputsCount, std::vector<std::string> outputsCount, bool multiEdgesInput, bool multiEdgesOutput)
    : scene(scene), title(nodeName), m_MultiEdgesInput(multiEdgesInput), m_MultiEdgesOutput(multiEdgesOutput)
{
    nodeContent  = new NodeContentWidget;
    graphicsNode = new GraphicsNode(this);

    scene->addNode(this);
    scene->getGraphicsScene()->addItem(graphicsNode);

    for (uint32_t i = 0; i < inputsCount.size(); i++) {
        auto socket = new Socket(this, SOCKET_INPUT, i, style, LEFT_BOTTOM, m_MultiEdgesInput, inputsCount[i]);
        inputs.push_back(socket);
    }

    for (uint32_t i = 0; i < outputsCount.size(); i++) {
        auto socket = new Socket(this, SOCKET_OUTPUT, i, style, RIGHT_TOP, m_MultiEdgesOutput, outputsCount[i]);
        outputs.push_back(socket);
    }
}

void Node::setPos(uint32_t x, uint32_t y)
{
    graphicsNode->setPos(x, y);
}

void Node::setTitle(const std::string& title)
{
    this->title = title;
    graphicsNode->getTextItem()->setPlainText(title.c_str());
    graphicsNode->update();
}

void Node::setGraphicsNode(IGraphicsNode* grNode)
{
    auto oldPos = graphicsNode->pos();
    // remove old node
    scene->getGraphicsScene()->removeItem(graphicsNode);
    delete graphicsNode;

    // FIXME: Instead of clearing add the old sockets again

    inputs.clear();
    outputs.clear();

    graphicsNode = grNode;
    scene->getGraphicsScene()->addItem(grNode);

    graphicsNode->setPos(oldPos);
    graphicsNode->update();
}

QPointF Node::getPos()
{
    return graphicsNode->pos();
}

QPointF Node::getSocketPosition(uint32_t index, SocketPos pos)
{
    if (!graphicsNode)
        return QPointF();

    float x = 0, y = 0;
    if (pos == LEFT_TOP || pos == LEFT_BOTTOM) x = 0;
    else
        x = graphicsNode->getWidth();

    if (pos == LEFT_TOP || pos == RIGHT_TOP)
        y = graphicsNode->getTitleHeight() + graphicsNode->getEdgeSize() + index * socketSpacing + graphicsNode->getPadding();
    else if (pos == LEFT_BOTTOM || pos == RIGHT_BOTTOM)
        y = graphicsNode->getHeight() - graphicsNode->getEdgeSize() - index * socketSpacing - graphicsNode->getPadding();

    return QPointF(x, y);
}

void Node::remove()
{
    for (auto input: inputs) {
        for (auto edge: input->getEdges())
            edge->remove();
    }

    for (auto output: outputs) {
        for (auto edge: output->getEdges())
            edge->remove();
    }
    // Doing remove item instead of delete
    //graphicsNode->~GraphicsNode();
    //scene->getGraphicsScene()->removeItem(graphicsNode);
    delete graphicsNode;
    graphicsNode = nullptr;
    scene->removeNode(this);
}

void Node::add()
{
#if 0
    for (auto input : inputs) {
        if (input->hasEdges())
            input->getEdge()->add();
    }

    for (auto output : outputs) {
        if (output->hasEdges())
            output->getEdge()->add();
    }
#endif

    //for (size_t i = 0; i < inputs.size(); i++) {
    //    auto socket = new Socket(this, i, LEFT_BOTTOM, inputs[i]);
    //    inputs.push_back(socket);
    //}

    //for (size_t i = 0; i < outputs.size(); i++) {
    //    auto socket = new Socket(this, i, RIGHT_TOP);
    //    outputs.push_back(socket);
    //}

    graphicsNode = new GraphicsNode(this);

    scene->addNode(this);
    scene->getGraphicsScene()->addItem(graphicsNode);
}

void Node::update()
{
    graphicsNode->update();
}

void Node::addInputSocket(const std::string& name /*= "input_socket"*/, SocketStyle style /*= CIRCLE*/, SocketPos pos /*= LEFT_TOP*/, const std::string& hexColor /*= "FFFF7700"*/)
{
    auto socket = new Socket(this, SOCKET_INPUT, (uint32_t) inputs.size(), style, pos, m_MultiEdgesInput, hexColor, name);
    inputs.push_back(socket);

    graphicsNode->update();
}

void Node::removeInputSocket(uint32_t idx)
{
    auto socket = getInputSocket(idx);
    socket->remove();

    inputs.erase(inputs.begin() + idx);
    graphicsNode->update();
}

void Node::addOutputSocket(const std::string& name /*= "input_socket"*/, SocketStyle style /*= CIRCLE*/, SocketPos pos /*= RIGHT_TOP*/, const std::string& hexColor /*= "#00A5FF"*/)
{
    auto socket = new Socket(this, SOCKET_OUTPUT, (uint32_t) outputs.size(), style, pos, m_MultiEdgesOutput, hexColor, name);
    outputs.push_back(socket);

    graphicsNode->update();
}

void Node::removeOutputSocket(uint32_t idx)
{
    auto socket = getOutputSocket(idx);
    socket->remove();

    outputs.erase(outputs.begin() + idx);
    graphicsNode->update();
}

AddNodeCommand::AddNodeCommand(Node* node, QGraphicsScene* scene)
    : mNode(node), mGraphicsScene(scene)
{
    mNode            = node;
    mInitialPosition = node->getPos();
    mGraphicsScene->update();

    setText("Added node");
}

void AddNodeCommand::undo()
{
    mNode->remove();
    mGraphicsScene->update();
}

void AddNodeCommand::redo()
{
    mNode->add();
    mNode->setPos(mInitialPosition.x(), mInitialPosition.y());
    mGraphicsScene->clearSelection();
    mGraphicsScene->update();
}

RemoveNodeCommand::RemoveNodeCommand(QGraphicsScene* scene)
    : mGraphicsScene(scene)
{
    QList<QGraphicsItem*> list = scene->selectedItems();
    if (list.size() > 0) {
        list.first()->setSelected(false);

        mNode            = static_cast<GraphicsNode*>(list.first())->getNode();
        mInitialPosition = mNode->getPos();
        mGraphicsScene->update();

        setText("Removed node");
    }
}

void RemoveNodeCommand::undo()
{
    mNode->add();
    mNode->setPos(mInitialPosition.x(), mInitialPosition.y());
    mGraphicsScene->clearSelection();
    mGraphicsScene->update();
}

void RemoveNodeCommand::redo()
{
    mNode->remove();
    mGraphicsScene->update();
}

MoveNodeCommand::MoveNodeCommand(Node* node, QPointF oldPos, QGraphicsScene* scene)
    : mNode(node), mOldPosition(oldPos), mNewPosition(node->getPos()), mGraphicsScene(scene)

{
    setText("Moved node");
}

void MoveNodeCommand::undo()
{
    mNode->setPos(mOldPosition.x(), mOldPosition.y());
    mGraphicsScene->update();
}

void MoveNodeCommand::redo()
{
    mNode->setPos(mNewPosition.x(), mNewPosition.y());
    mGraphicsScene->update();
}

bool MoveNodeCommand::mergeWith(const QUndoCommand* other)
{
    const MoveNodeCommand* moveCommand = static_cast<const MoveNodeCommand*>(other);
    Node*                  item        = moveCommand->mNode;

    if (item != mNode)
        return false;

    mNewPosition = item->getPos();

    return true;
}
