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

            RAZIX_INLINE u32  getSize() const { return m_Size; }
            RAZIX_INLINE void setSize(u32 val) { m_Size = val; }

            RAZIX_INLINE std::string getUsage() const { return m_Usage; }
            RAZIX_INLINE void        setUsage(std::string val) { m_Usage = val; }

            RAZIX_INLINE bool getEnableReflectionData() const { return m_EnableReflectionData; }
            RAZIX_INLINE void setEnableReflectionData(bool val) { m_EnableReflectionData = val; }

        private:
            BufferResourceNodeGraphicsNode* m_BufferResourceGraphicsNode = nullptr;
            //------------------------------
            u32         m_Size = 0;
            std::string m_Usage;
            bool        m_EnableReflectionData = false;
        };

        //-----------------------------------------------------------------------------------

        //REGISTER_NODE(BufferResourceNode);

    }    // namespace Editor
}    // namespace Razix