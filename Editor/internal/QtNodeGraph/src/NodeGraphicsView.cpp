#include "NodeGraphicsView.h"

#include "GraphicsNode.h"
#include "Node.h"
#include "NodeScene.h"

NodeGraphicsView::NodeGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
}

void NodeGraphicsView::init(NodeScene* scene)
{
    m_Scene = scene;

    setScene(scene->getGraphicsScene());

    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // Enable selection using mouse
    setDragMode(QGraphicsView::RubberBandDrag);

    // End DragnDrop
    setAcceptDrops(true);

    m_Cutline = new GraphicsCutLine();
    scene->getGraphicsScene()->addItem(m_Cutline);
    //m_Cutline->setPos(scene->getOrigin());
    auto pos = m_Cutline->pos();
    auto bbr = m_Cutline->boundingRect();
}

void NodeGraphicsView::setNodeScene(NodeScene* scene)
{
    m_Scene = scene;
    setScene(m_Scene->getGraphicsScene());
}

void NodeGraphicsView::deleteSelected()
{
    auto& selectedItems = m_Scene->getGraphicsScene()->selectedItems();
    for each (auto item in m_Scene->getGraphicsScene()->selectedItems()) {
        if (!item)
            return;

        if (std::find(selectedItems.begin(), selectedItems.end(), item) == selectedItems.end())
            return;

        if (dynamic_cast<GraphicsEdge*>(item)) {
            dynamic_cast<GraphicsEdge*>(item)->getEdge()->remove();
            selectedItems.erase(std::remove(selectedItems.begin(), selectedItems.end(), item), selectedItems.end());
            item = nullptr;
        } else if (dynamic_cast<IGraphicsNode*>(item)) {
            //dynamic_cast<GraphicsNode*>(item)->getNode()->remove();
            m_Scene->getUndoStack()->push(new RemoveNodeCommand(m_Scene->getGraphicsScene()));
            selectedItems.erase(std::remove(selectedItems.begin(), selectedItems.end(), item), selectedItems.end());
            item = nullptr;
        }
    }
}

void NodeGraphicsView::cutIntersectingEdges()
{
}
