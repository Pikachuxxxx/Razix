#include "GraphicsNode.h"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsProxyWidget>

#include "Node.h"
#include "NodeContentWidget.h"
#include "NodeScene.h"

GraphicsNode::GraphicsNode(Node* node)
    : IGraphicsNode(node)
{
}

//void GraphicsNode::initContent()
//{
//    auto grContent = new QGraphicsProxyWidget(this);
//    node->getContent()->setGeometry(QRect(edge_size, titleHeight + edge_size, width - 2 * edge_size, height - 2 * edge_size - titleHeight));
//    grContent->setWidget(node->getContent());
//    grContent->setZValue(10);
//}

void GraphicsNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // Title
    auto path_title = QPainterPath();
    path_title.setFillRule(Qt::WindingFill);
    path_title.addRoundedRect(0, 0, width, titleHeight, edge_size, edge_size);
    path_title.addRect(0, titleHeight - edge_size, edge_size, edge_size);
    path_title.addRect(width - edge_size, titleHeight - edge_size, edge_size, edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(titleBrush);
    painter->drawPath(path_title.simplified());

    // Content
    auto path_content = QPainterPath();
    path_content.setFillRule(Qt::WindingFill);
    path_content.addRoundedRect(0, titleHeight, width, height - titleHeight, edge_size, edge_size);
    path_content.addRect(0, titleHeight, edge_size, edge_size);
    path_content.addRect(width - edge_size, titleHeight, edge_size, edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgBrush);
    painter->drawPath(path_content.simplified());

    // Outline
    auto path_outline = QPainterPath();
    path_outline.addRoundedRect(0, 0, width, height, edge_size, edge_size);
    painter->setBrush(Qt::NoBrush);
    if (hovered) {
        painter->setPen(penHovered);
        painter->drawPath(path_outline.simplified());
    }

    if (!isSelected())
        painter->setPen(penDefault);
    else
        painter->setPen(penSelected);
    painter->drawPath(path_outline.simplified());
}

IGraphicsNode::IGraphicsNode(Node* node)
    : node(node)
{
    this->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    this->setPos(node->getScene()->getOrigin().x(), node->getScene()->getOrigin().y());

    titleItem = new QGraphicsTextItem(this);
    QFont f;
    f.setPointSize(12);
    f.setBold(false);
    titleItem->setFont(f);
    titleItem->setPlainText(node->getTitle().c_str());
    titleItem->setDefaultTextColor(Qt::white);
    titleItem->setTextWidth(width - 2 * padding);
    titleItem->setPos(padding, 0);
    penHovered = QPen("#FF37A6FF");
    penHovered.setWidth(6.0f);

    penDefault = QPen(QColor("#000000"));    // ARGB E5FA96
    // florescent green - RGB
    //penDefault = QPen(QColor("#FFE5FA96"));
    penDefault.setWidth(4);
    penSelected = QPen(QColor("#FFFFA637"));
    penSelected.setWidth(4);

    titleBrush = QBrush(QColor("#FF313131"));
    //titleBrush = QBrush(QColor("#FFB9F027"));
    bgBrush = QBrush(QColor("#E3212121"));
    //bgBrush = QBrush(QColor("#FF84AD18"));
    setAcceptHoverEvents(true);

    QGraphicsDropShadowEffect* DropShadowEffect = new QGraphicsDropShadowEffect();
    DropShadowEffect->setBlurRadius(20);
    DropShadowEffect->setOffset(1, 2);
    DropShadowEffect->setColor(Qt::black);

    this->setGraphicsEffect(DropShadowEffect);
}

void IGraphicsNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    m_NodeOldPos = pos();

    QGraphicsItem::mousePressEvent(event);
}

void IGraphicsNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (pos() != m_NodeOldPos)
        node->getScene()->getUndoStack()->push(new MoveNodeCommand(node, m_NodeOldPos, node->getScene()->getGraphicsScene()));

    QGraphicsItem::mouseReleaseEvent(event);
}