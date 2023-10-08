#pragma once

#include <QtNodeGraph.h>

namespace Razix {
    namespace Editor {

        class PassNodeGraphicsNode : public IGraphicsNode
        {
        public:
            PassNodeGraphicsNode(Node* node);
            ~PassNodeGraphicsNode();

            QRectF boundingRect() const override;    // { return QRectF(0, 0, width, height).normalized(); }
            void   paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /* = nullptr */) override;
        };

        //-----------------------------------------------------------------------------------

        class RZEPassNodeUI : public Node
        {
        public:
            RZEPassNodeUI(const std::string& nodeName, NodeScene* scene);
            ~RZEPassNodeUI();

        private:
            PassNodeGraphicsNode* m_PassGraphicsNode = nullptr;
        };

        //-----------------------------------------------------------------------------------

        //REGISTER_NODE(PassNode);

    }    // namespace Editor
}    // namespace Razix