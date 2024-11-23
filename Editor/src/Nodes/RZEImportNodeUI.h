#pragma once

#include <QtNodeGraph.h>

namespace Razix {
    namespace Editor {

        class ImportNodeGraphicsNode : public IGraphicsNode
        {
        public:
            ImportNodeGraphicsNode(Node* node);
            ~ImportNodeGraphicsNode();

            QRectF boundingRect() const override;    // { return QRectF(0, 0, width, height).normalized(); }
            void   paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /* = nullptr */) override;
        };

        //-----------------------------------------------------------------------------------

        class RZEImportNodeUI : public Node
        {
        public:
            RZEImportNodeUI(const std::string& nodeName, NodeScene* scene);
            ~RZEImportNodeUI();

            std::string m_TexturePath;

        private:
            ImportNodeGraphicsNode* m_ImportGraphicsNode = nullptr;
        };

        //-----------------------------------------------------------------------------------

        //REGISTER_NODE(ImportNode);

    }    // namespace Editor
}    // namespace Razix