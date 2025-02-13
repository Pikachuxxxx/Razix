#if 1
    #pragma once

    #include "Razix/Gfx/Renderers/IRZRendererProxy.h"

    #include <imgui/plugins/IconsFontAwesome5.h>

    #include <glm/glm.hpp>

namespace Razix {

    class RZTimestep;

    namespace Gfx {

        class RZDrawCommandBuffer;
        class RZVertexBuffer;
        class RZIndexBuffer;
        class RZPipeline;
        class RZShader;
        class RZTexture;
        class RZDescriptorSet;

        // - [ ] Separate Texture and Sampler for font texture in Shader

        /**
         * ImGui renderer proxy for the Razix engine, manages everything necessary for UI elements
         * 
         * Unlike IRZRendererProxy this is a standalone renderer proxy that implements ImGui for different Rendering APIs 
         * with a common architecture to be used in frame graph more robustly and independently
         * 
         * Note: Used GLFW for events, once engine wide common Input-platform system is done we can use that to redirect events to ImGui controls 
         * such as for consoles etc.
         */
        class RAZIX_API RZImGuiRendererProxy : public IRZRendererProxy
        {
        private:
            struct PushConstant
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
            RZImGuiRendererProxy() {}
            ~RZImGuiRendererProxy() {}

            void Init() override;
            void Begin(RZScene* scene) override;
            void Draw(RZDrawCommandBufferHandle cmdBuffer) override;
            void End() override;
            void Resize(u32 width, u32 height) override;
            void Destroy() override;

        private:
            RZDescriptorSetHandle m_FontAtlasDescriptorSet = {};
            RZTextureHandle       m_FontAtlasTexture       = {};
            RZVertexBufferHandle  m_ImGuiVBO               = {};
            RZIndexBufferHandle   m_ImGuiIBO               = {};
            RZShaderHandle        m_ImGuiShader            = {};
            PushConstant          m_PushConstantData       = {};

        private:
            void setupImGuiContext();
            void setupImGuiFlags();
            void setupImGuiStyle();
            void setupResources();
            void loadImGuiFonts();
            void uploadCustomUIFont(const std::string& fontPath);
        };
    }    // namespace Gfx
}    // namespace Razix
#endif