#pragma once

#include <QtNodeGraph.h>

namespace Razix {
    namespace Editor {

        class BufferResourceNodeGraphicsNode : public IGraphicsNode
        {
        public:
            BufferResourceNodeGraphicsNode(Node* node);
            ~BufferResourceNodeGraphicsNode();

            QRectF boundingRect() const override;    // { return QRectF(0, 0, width, height).normalized(); }
            void   paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /* = nullptr */) override;
        };

        //-----------------------------------------------------------------------------------

        class RZEBufferResourceNodeUI : public Node
        {
        public:
            RZEBufferResourceNodeUI(const std::string& nodeName, NodeScene* scene);
            ~RZEBufferResourceNodeUI();

        private:
            BufferResourceNodeGraphicsNode* m_BufferResourceGraphicsNode = nullptr;
        };

        //-----------------------------------------------------------------------------------

        //REGISTER_NODE(BufferResourceNode);

    }    // namespace Editor
}    // namespace Razix