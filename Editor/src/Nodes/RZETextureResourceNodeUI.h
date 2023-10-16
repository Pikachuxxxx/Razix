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

            // TODO: Make them private add getters/setters
            //------------------------------
            u32         m_Width  = 0;
            u32         m_Height = 0;
            u32         m_Depth  = 0;
            u32         m_Layers = 0;
            std::string m_Data;
            std::string m_Type;
            std::string m_Format;
            std::string m_Wrapping;
            std::string m_FilteringMin;
            std::string m_FilteringMag;
            bool        m_EnableMips = false;
            bool        m_IsHDR      = false;

        private:
            TextureResourceNodeGraphicsNode* m_TextureResourceGraphicsNode = nullptr;
        };

        //-----------------------------------------------------------------------------------

        //REGISTER_NODE(TextureResourceNode);

    }    // namespace Editor
}    // namespace Razix