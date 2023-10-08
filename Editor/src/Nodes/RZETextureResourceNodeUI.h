#pragma once

#include <QtNodeGraph.h>

namespace Razix {
    namespace Editor {

        class TextureResourceNodeGraphicsNode : public IGraphicsNode
        {
        public:
            TextureResourceNodeGraphicsNode(Node* node);
            ~TextureResourceNodeGraphicsNode();

            QRectF boundingRect() const override;    // { return QRectF(0, 0, width, height).normalized(); }
            void   paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /* = nullptr */) override;
        };

        //-----------------------------------------------------------------------------------

        class RZETextureResourceNodeUI : public Node
        {
        public:
            RZETextureResourceNodeUI(const std::string& nodeName, NodeScene* scene);
            ~RZETextureResourceNodeUI();

        private:
            TextureResourceNodeGraphicsNode* m_TextureResourceGraphicsNode = nullptr;
        };

        //-----------------------------------------------------------------------------------

        //REGISTER_NODE(TextureResourceNode);

    }    // namespace Editor
}    // namespace Razix