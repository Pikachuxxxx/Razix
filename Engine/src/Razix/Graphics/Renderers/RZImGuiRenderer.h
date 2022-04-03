#pragma once

#include "Razix/Graphics/Renderers/IRZRenderer.h"

#include <imgui.h>

namespace Razix {

    namespace Graphics {
        
        /**
         * ImGui renderer for the Razix engine, manages everything necessary
         * 
         * Unlike IRZRenderer this is a standalone renderer that manages ImGui for different Rendering APIs with a common architecture similar to RZAPIRenderer
         * 
         * Note: Used GLFW for events, once engine wide common Input-platform system is done we can use that to redirect events to ImGui controls 
         * such as for consoles etc.
         */
        class RZImGuiRenderer
        {
        public:
            static RZImGuiRenderer* Create(uint32_t width, uint32_t height);

            virtual ~RZImGuiRenderer() = default;

            virtual void Init() = 0;
            virtual void NewFrame() = 0;
            virtual void Render(RZCommandBuffer * commandBuffer) = 0;
            virtual void EndFrame() = 0;
            virtual void OnResize(uint32_t width, uint32_t height) = 0;
            virtual void RebuildFontTexture() = 0;
        };

    }
}