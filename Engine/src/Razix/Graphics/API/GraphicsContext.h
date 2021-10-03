#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/Log.h"

#include "Razix/Core/OS/Window.h"

namespace Razix
{
    namespace Graphics {

        /* The Render API to use for rendering the application */
        enum class RenderAPI
        {
            OPENGL = 0,
            VULKAN,
            DIRECTX11,
            DIRECTX12,  // Not Supported yet!
            GXM,        // Not Supported yet! (PSVita)
            GCM         // Not Supported yet! (PS3)
        };

        /**
         * The Graphics Context that manages the context of the underlying graphics API
         */
        class RAZIX_API GraphicsContext
        {
        public:
            static void Create(const WindowProperties& properties, Window* window);
            static void Release();

            virtual void Init() = 0;
            virtual void SwapBuffers() = 0;
            virtual void ClearWithColor(float r, float g, float b) = 0;

            static GraphicsContext* Get();

            static RenderAPI GetRenderAPI() { return s_RenderAPI; }
            static void SetRenderAPI(RenderAPI api) { s_RenderAPI = api; }
            static const std::string GetRenderAPIString();

        protected:
            /* Pointer to the underlying graphics APi implementation */
            static GraphicsContext* s_Context;
            /* The render API selected to render the application */
            static RenderAPI s_RenderAPI;
        };
    }
}