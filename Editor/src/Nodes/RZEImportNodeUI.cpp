// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEImportNodeUI.h"

namespace Razix {
    namespace Editor {

        ImportNodeGraphicsNode::ImportNodeGraphicsNode(Node* node)
            : IGraphicsNode(node)
        {
            width  = 240;
            height = 40;

            titleHeight = 0;

            penDefault.setStyle(Qt::DashLine);
            penDefault.setWidthF(2.0f);

            QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, height));
            linearGrad.setColorAt(1.0, QColor("#586687"));
            linearGrad.setColorAt(0.0, QColor("#B5CDFA"));

            bgBrush = QBrush(linearGrad);

            titleItem->setDefaultTextColor(Qt::black);
            titleItem->setTextWidth(-1);

            QFontMetrics fm(titleItem->font());

            titleItem->setPos(width / 2 - fm.horizontalAdvance(titleItem->toPlainText()) / 2, 4);
        }

        ImportNodeGraphicsNode::~ImportNodeGraphicsNode()
        {
        }

        QRectF ImportNodeGraphicsNode::boundingRect() const
        {
            return QRectF(0, 0, width, height).normalized();
        }

        void ImportNodeGraphicsNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /* = nullptr */)
        {
            // Content
            auto path_content = QPainterPath();
            path_content.setFillRule(Qt::WindingFill);
            path_content.addRoundedRect(0, titleHeight, width, height, edge_size, edge_size);
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

        //-----------------------------------------------------------------------------------

        RZEImportNodeUI::RZEImportNodeUI(const std::string& nodeName, NodeScene* scene)
            : Node(scene, nodeName, PIN, 0, 0)
        {
            m_ImportGraphicsNode = new ImportNodeGraphicsNode(this);
            this->setGraphicsNode(m_ImportGraphicsNode);

            addOutputSocket("", PIN);
        }

        RZEImportNodeUI::~RZEImportNodeUI()
        {
        }
    }    // namespace Editor
}    // namespace Razix