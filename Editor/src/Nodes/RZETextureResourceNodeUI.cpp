// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZETextureResourceNodeUI.h"

namespace Razix {
    namespace Editor {

        TextureResourceNodeGraphicsNode::TextureResourceNodeGraphicsNode(Node* node)
            : IGraphicsNode(node)
        {
            width  = 240;
            height = 40;

            titleHeight = 0;

            penDefault.setStyle(Qt::DotLine);
            penDefault.setWidthF(1.0f);
            penDefault.setDashOffset(0.5f);
            bgBrush = QBrush(QColor("#9AD0FC"));

            titleItem->setDefaultTextColor(Qt::black);
            titleItem->setTextWidth(-1);

            QFontMetrics fm(titleItem->font());

            titleItem->setPos(width / 2 - fm.horizontalAdvance(titleItem->toPlainText()) / 2, 4);

            QLinearGradient rainbowGrad(QPointF(0, 0), QPointF(width, height));
            rainbowGrad.setColorAt(1.0, QColor("#FF0000"));
            rainbowGrad.setColorAt(0.5, QColor("#00FF00"));
            rainbowGrad.setColorAt(0.0, QColor("#0000FF"));

            outputPen = QPen(rainbowGrad, 5);
        }

        TextureResourceNodeGraphicsNode::~TextureResourceNodeGraphicsNode()
        {
        }

        QRectF TextureResourceNodeGraphicsNode::boundingRect() const
        {
            return QRectF(0, 0, width, height).normalized();
        }

        void TextureResourceNodeGraphicsNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /* = nullptr */)
        {
            // Content
            auto path_content = QPainterPath();
            path_content.setFillRule(Qt::WindingFill);
            path_content.addRoundedRect(0, titleHeight, width, height, edge_size, edge_size);
            painter->setPen(Qt::NoPen);
            painter->setBrush(bgBrush);
            painter->drawPath(path_content.simplified());

            if (m_IsFinalOutput) {
                auto path_outline = QPainterPath();
                path_outline.addRoundedRect(-10, -10, width + 20, height + 20, edge_size, edge_size);
                painter->setBrush(Qt::NoBrush);
                painter->setPen(outputPen);
                painter->drawPath(path_outline.simplified());
            }

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

        RZETextureResourceNodeUI::RZETextureResourceNodeUI(const std::string& nodeName, NodeScene* scene)
            : Node(scene, nodeName, PIN, 1, 1)
        {
            m_TextureResourceGraphicsNode = new TextureResourceNodeGraphicsNode(this);
            this->setGraphicsNode(m_TextureResourceGraphicsNode);

            addInputSocket("", PIN);
            addOutputSocket("", PIN);
        }

        RZETextureResourceNodeUI::~RZETextureResourceNodeUI()
        {
        }

    }    // namespace Editor
}    // namespace Razix