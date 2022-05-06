#pragma once

#include "Razix/Graphics/Renderers/IRZRenderer.h"

#include "Razix/Graphics/API/RZCommandBuffer.h"
#include "Razix/Graphics/API/RZIndexBuffer.h"
#include "Razix/Graphics/API/RZPipeline.h"
#include "Razix/Graphics/API/RZRenderPass.h"
#include "Razix/Graphics/API/RZVertexBuffer.h"
#include "Razix/Graphics/API/RZVertexBufferLayout.h"

#include "Razix/Utilities/RZTimestep.h"

// Imgui
#include <imgui.h>
#include <imgui_internal.h>

namespace Razix {

    namespace Graphics {

        /**
         * ImGui renderer for the Razix engine, manages everything necessary for UI elements
         * 
         * Unlike IRZRenderer this is a standalone renderer that manages ImGui for different Rendering APIs with a common architecture similar to RZAPIRenderer
         * 
         * Note: Used GLFW for events, once engine wide common Input-platform system is done we can use that to redirect events to ImGui controls 
         * such as for consoles etc.
         */
        class RAZIX_API RZImGuiRenderer
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

        public:
            PushConstBlock pushConstBlock;
            RZImGuiRenderer () {}
            ~RZImGuiRenderer (){};

            void init ();
            void createPipeline (RZRenderPass& renderpass);
            bool update (const RZTimestep& dt);
            void draw (RZCommandBuffer* cmdBuffer);
            void destroy ();

        private:
            RZShader*        m_UIShader; /* The ImGui shader that is used by ImGui to render the UI elements */
            RZTexture2D*     m_FontAtlasTexture;
            RZDescriptorSet* m_FontAtlasDescriptorSet;
            RZPipeline*      m_ImGuiPipeline;

            RZVertexBuffer* m_ImGuiVBO = nullptr;
            RZIndexBuffer*  m_ImGuiIBO = nullptr;

            int32_t vertexCount = 0;
            int32_t indexCount  = 0;

            std::vector<ImGuiVertex> m_VertexData;
            std::vector<uint16_t>    m_IndexData;
            RZVertexBufferLayout     layout;

        private:
            void uploadUIFont (const std::string& fontPath);
        };

    }    // namespace Graphics
}    // namespace Razix