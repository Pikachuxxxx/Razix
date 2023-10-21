#pragma once

#include <QtNodeGraph.h>

namespace Razix {
    namespace Editor {

        struct PipelineDescStringView
        {
            std::string              pipelineName;
            std::string              cullMode;
            std::string              polygonMode;
            std::string              drawMode;
            bool                     enableTransparencey;
            bool                     enableDepthTest;
            bool                     enableDepthWrite;
            std::string              depthOperation;
            std::string              colorSrc;
            std::string              colorDst;
            std::string              colorOp;
            std::string              alphaSrc;
            std::string              alphaDst;
            std::string              alphaOp;
            std::string              depthFormat;
            std::vector<std::string> colorFormats;
        };

        struct SceneParamsStringView
        {
            std::string geometryMode;
            bool        enableResize;
            std::string resolution;
            glm::vec2   extents;
            int         layers;
        };

        class PassNodeGraphicsNode : public IGraphicsNode
        {
        public:
            PassNodeGraphicsNode(Node* node);
            ~PassNodeGraphicsNode();

            QRectF boundingRect() const override;    // { return QRectF(0, 0, width, height).normalized(); }
            void   paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /* = nullptr */) override;

            bool getHasBreakPoint() const { return m_HasBreakPoint; }
            void setHasBreakPoint(bool val) { m_HasBreakPoint = val; }

            bool getIsStandAlonePass() const { return m_IsStandAlonePass; }
            void setIsStandAlonePass(bool val) { m_IsStandAlonePass = val; }

        private:
            u32                titleBgHeight      = 30;
            QGraphicsTextItem* m_IdxTitle         = nullptr;
            bool               m_HasBreakPoint    = false;
            bool               m_IsStandAlonePass = false;
        };

        //-----------------------------------------------------------------------------------

        class RZEPassNodeUI : public Node
        {
        public:
            static u32 Idx;

            RZEPassNodeUI(const std::string& nodeName, NodeScene* scene);
            ~RZEPassNodeUI();

            RAZIX_INLINE const std::string& getShaderName() { return m_ShaderName; }
            RAZIX_INLINE void               setShaderName(const std::string& name) { m_ShaderName = name; }

            RAZIX_INLINE PipelineDescStringView& getPipelineSettings() { return m_PipelineSettings; }
            RAZIX_INLINE SceneParamsStringView&  getSceneSettings() { return m_SceneSettings; }

            bool getHasBreakPoint() const { return m_HasBreakPoint; }
            void setHasBreakPoint(bool val)
            {
                m_HasBreakPoint = val;
                m_PassGraphicsNode->setHasBreakPoint(val);
            }

            bool getIsStandAlonePass() const { return m_IsStandAlonePass; }
            void setIsStandAlonePass(bool val)
            {
                m_IsStandAlonePass = val;
                m_PassGraphicsNode->setIsStandAlonePass(val);
            }

        private:
            PassNodeGraphicsNode* m_PassGraphicsNode = nullptr;
            //------------------------------
            // Name - Node
            // Sockets info - Node
            std::string            m_ShaderName;
            PipelineDescStringView m_PipelineSettings{};
            SceneParamsStringView  m_SceneSettings{};
            bool                   m_HasBreakPoint    = false;
            bool                   m_IsStandAlonePass = false;
        };

        //-----------------------------------------------------------------------------------

        //REGISTER_NODE(PassNode);

    }    // namespace Editor
}    // namespace Razix