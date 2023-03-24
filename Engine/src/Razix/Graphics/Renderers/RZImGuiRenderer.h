#if 1
    #pragma once

    #include "Razix/Graphics/Renderers/IRZRenderer.h"

    #include <imgui/plugins/IconsFontAwesome5.h>

    #include <glm/glm.hpp>

namespace Razix {

    class RZTimestep;

    namespace Graphics {

        class RZCommandBuffer;
        class RZVertexBuffer;
        class RZIndexBuffer;
        class RZPipeline;
        class RZShader;
        class RZTexture2D;
        class RZDescriptorSet;

        /**
         * ImGui renderer for the Razix engine, manages everything necessary for UI elements
         * 
         * Unlike IRZRenderer this is a standalone renderer that manages ImGui for different Rendering APIs with a common architecture similar to RZAPIRenderer
         * 
         * Note: Used GLFW for events, once engine wide common Input-platform system is done we can use that to redirect events to ImGui controls 
         * such as for consoles etc.
         */
        class RAZIX_API RZImGuiRenderer : public IRZRenderer
        {
        public:
            struct PushConstBlock
            {
                glm::vec2 scale;
                glm::vec2 translate;
            };

            struct ImGuiVertex
            {
                glm::vec2 pos;
                glm::vec2 uv;
                glm::vec4 color;
            };

            PushConstBlock pushConstBlock;

            void Init() override;

            void Begin(RZScene* scene) override;

            void Draw(RZCommandBuffer* cmdBuffer) override;

            void End() override;

            void Resize(u32 width, u32 height) override;

            void Destroy() override;

        public:
            RZImGuiRenderer() {}
            ~RZImGuiRenderer() {}

        private:
            RZTexture2D*     m_FontAtlasTexture;
            RZDescriptorSet* m_FontAtlasDescriptorSet;

            RZVertexBuffer* m_ImGuiVBO = nullptr;
            RZIndexBuffer*  m_ImGuiIBO = nullptr;

            //int32_t vertexCount = 0;
            //int32_t indexCount  = 0;

            std::vector<ImGuiVertex> m_VertexData;
            std::vector<u16>    m_IndexData;

        private:
            void initDisposableResources();
            void uploadUIFont(const std::string& fontPath);
        };

    }    // namespace Graphics
}    // namespace Razix
#endif