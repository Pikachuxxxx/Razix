// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEPassNodeUI.h"

#include "Razix/Utilities/RZColorUtilities.h"

namespace Razix {
    namespace Editor {

        // https://www.color-hex.com/color-names.html

        PassNodeGraphicsNode::PassNodeGraphicsNode(Node* node)
            : IGraphicsNode(node)
        {
            width       = 240;
            height      = 60;
            titleHeight = 30;

            titleHeight = -4;

            QFontMetrics fm(titleItem->font());

            QFont f = titleItem->font();
            f.setPointSize(12);
            f.setBold(false);
            titleItem->setFont(f);

            titleItem->setPos(width / 2 - fm.horizontalAdvance(titleItem->toPlainText()) / 2, 4);

            // https://twitter.com/colorswall/status/1211954196980936704/photo/1
            //QLinearGradient redLinearGrad(QPointF(0, 0), QPointF(0, width));
            //redLinearGrad.setColorAt(1.0, QColor("#0e0000"));
            //redLinearGrad.setColorAt(0.9, QColor("#1c0000"));
            //redLinearGrad.setColorAt(0.8, QColor("#2a0000"));
            //redLinearGrad.setColorAt(0.7, QColor("#380000"));
            //redLinearGrad.setColorAt(0.6, QColor("#460000"));
            //redLinearGrad.setColorAt(0.5, QColor("#530000"));
            //redLinearGrad.setColorAt(0.4, QColor("#610000"));
            //redLinearGrad.setColorAt(0.3, QColor("#6f0000"));
            //redLinearGrad.setColorAt(0.2, QColor("#7d0000"));
            //redLinearGrad.setColorAt(0.1, QColor("#8b0000"));

            auto randomColor = Razix::Utilities::GenerateHashedColor(7 + rand());
            titleBrush       = QBrush(QColor(randomColor.r * 255, randomColor.g * 255, randomColor.b * 255, 255));
            penDefault       = QPen(titleBrush.color());
            penDefault.setWidth(2);
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
            uint32_t socketsCount = uint32_t(std::max(node->getInputSockets().size(), node->getOutputSockets().size()));
            if (socketsCount > 3)
                height = socketsCount * SOCKET_SPACING;

#if 1
            // Title
            auto path_title = QPainterPath();
            path_title.setFillRule(Qt::WindingFill);
            path_title.addRoundedRect(0, 0, width, titleBgHeight, edge_size, edge_size);
            path_title.addRect(0, titleBgHeight - edge_size, edge_size, edge_size);
            path_title.addRect(width - edge_size, titleBgHeight - edge_size, edge_size, edge_size);
            painter->setPen(Qt::NoPen);
            painter->setBrush(titleBrush);
            painter->drawPath(path_title.simplified());
#endif

            // Content
            auto path_content = QPainterPath();
            path_content.setFillRule(Qt::WindingFill);
            path_content.addRoundedRect(0, titleBgHeight, width, height - titleBgHeight, edge_size, edge_size);
            path_content.addRect(0, titleBgHeight, edge_size, edge_size);
            path_content.addRect(width - edge_size, titleBgHeight, edge_size, edge_size);
            painter->setPen(Qt::NoPen);
            painter->setBrush(bgBrush);
            painter->drawPath(path_content.simplified());

            // Draw References Count + Standalone status
#if 1
            painter->setPen(QPen(QColor("#FFFFFF")));
            auto label = "Index : " + std::to_string(RZEPassNodeUI::Idx);
            painter->drawText(QPoint(12, titleBgHeight + 18), label.c_str());
            QIcon   icon   = QIcon(":/rzeditor/fg_toolbar_icons/mark_standalone_pass.png");
            QPixmap pixmap = icon.pixmap(QSize(22, 22), m_IsStandAlonePass ? QIcon::Normal : QIcon::Disabled);

            QFontMetrics fm(titleItem->font());
            painter->drawPixmap(fm.horizontalAdvance(label.c_str()) + 4, titleBgHeight + 8, pixmap);
#endif
            // TODO: Draw Disabled status label with rounded outline

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

            // Draw Breakpoint circle (on top left)
            if (m_HasBreakPoint) {
                painter->setPen(Qt::NoPen);
                painter->setBrush(QBrush(QColor("#FF0000")));
                painter->drawEllipse(QRectF(0, -15, 30, 30));
            }
        }

        //-----------------------------------------------------------------------------------

        u32 RZEPassNodeUI::Idx = 0;

        RZEPassNodeUI::RZEPassNodeUI(const std::string& nodeName, NodeScene* scene)
            : Node(scene, nodeName, PIN, 1, 1, true, true)
        {
            m_PassGraphicsNode = new PassNodeGraphicsNode(this);
            this->setGraphicsNode(m_PassGraphicsNode);

 
            addInputSocket("", HEADSHOT);
            addOutputSocket("", HEADSHOT);
        }

        RZEPassNodeUI::~RZEPassNodeUI()
        {
            Idx--;
        }
    }    // namespace Editor
}    // namespace Razix