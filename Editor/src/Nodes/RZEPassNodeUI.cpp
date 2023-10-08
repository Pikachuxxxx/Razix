// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEPassNodeUI.h"

namespace Razix {
    namespace Editor {

        PassNodeGraphicsNode::PassNodeGraphicsNode(Node* node)
            : IGraphicsNode(node)
        {
            width  = 240;
            height = 160;
        }

        PassNodeGraphicsNode::~PassNodeGraphicsNode()
        {
        }

        QRectF PassNodeGraphicsNode::boundingRect() const
        {
            return QRectF(0, 0, width, height).normalized();
        }

        void PassNodeGraphicsNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /* = nullptr */)
        {
            height = uint32_t(std::max(node->getInputSockets().size(), node->getOutputSockets().size())) * SOCKET_SPACING;

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

        //-----------------------------------------------------------------------------------

        RZEPassNodeUI::RZEPassNodeUI(const std::string& nodeName, NodeScene* scene)
            : Node(scene, nodeName, CIRCLE, 1, 1)
        {
            m_PassGraphicsNode = new PassNodeGraphicsNode(this);
            this->setGraphicsNode(m_PassGraphicsNode);

            addInputSocket("SceneHDR");
            addInputSocket("SceneDepth");
            addInputSocket("FrameData");
            addInputSocket("IrradianceMap");
            addInputSocket("PrefilteredMap");
            addInputSocket("BRDFLutMap");
            addInputSocket("SomeOtherMap");

            addOutputSocket("SceneHDR");
            addOutputSocket("SceneDepth");
        }

        RZEPassNodeUI::~RZEPassNodeUI()
        {
        }

    }    // namespace Editor
}    // namespace Razix