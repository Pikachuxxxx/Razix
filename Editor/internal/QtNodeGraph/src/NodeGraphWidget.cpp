#include "NodeGraphWidget.h"

#include <QBrush>
#include <QGraphicsItem>
#include <QPen>
#include <QVBoxLayout>

#include "Node.h"
#include "NodeEdge.h"
#include "Socket.h"

NodeGraphWidget::NodeGraphWidget(NodeGraphicsView* graphicsView, QWidget* parent)
    : QWidget(parent), m_GraphicsView(graphicsView)
{
    setGeometry(200, 200, 800, 600);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    m_Scene = new NodeScene;

    //m_GraphicsView = new NodeGraphicsView(m_Scene);
    if (!graphicsView)
        m_GraphicsView = new NodeGraphicsView();

    m_GraphicsView->init(m_Scene);
    m_GraphicsView->setNodeScene(m_Scene);
    layout->addWidget(m_GraphicsView);

    addDebugContent();
}

NodeGraphWidget::~NodeGraphWidget()
{
}

void NodeGraphWidget::addDebugContent()
{
#if 0
    QBrush greenBrush(Qt::green);
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(1.0f);

    auto rect = m_Scene->getGraphicsScene()->addRect(QRect(getOrigin().x(), getOrigin().y(), 300, 300), outlinePen, greenBrush);
    rect->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

    auto text = m_Scene->getGraphicsScene()->addText("Test text");
    text->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    text->setPos(getOrigin());
#endif

#if 0
    auto node   = new Node(m_Scene, "Test_Node_0", PIN, 2, 3);
    auto node_2 = new Node(m_Scene, "Test_Node_1", PIN, {"#00A5FF", "#00A5FF", "#00A5FF"}, {"#FFC0CB", "#FFC0CB"});
    auto node_3 = new Node(m_Scene, "Test_Node_2", PIN, {"#FFC0CB", "#FFC0CB"}, {"#F47174", "#F47174"});
    node->setPos(getOrigin().x() - 350, getOrigin().y() - 250);
    node_2->setPos(getOrigin().x() + 50, getOrigin().y() - 75);
    node_3->setPos(getOrigin().x() + 250, getOrigin().y() + 75);

    auto edge   = new NodeEdge(m_Scene, node->getOutputSocket(0), node_2->getInputSocket(1), BEZIER);
    auto edge_2 = new NodeEdge(m_Scene, node_2->getOutputSocket(1), node_3->getInputSocket(0), BEZIER);
#endif
}

QPoint NodeGraphWidget::getOrigin()
{
    return QPoint(m_Scene->getGraphicsScene()->sceneRect().width() / 2, m_Scene->getGraphicsScene()->sceneRect().height() / 2);
}
