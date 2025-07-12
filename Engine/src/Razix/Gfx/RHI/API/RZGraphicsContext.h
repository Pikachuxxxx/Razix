#pragma once

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/RZCore.h"

#include "Razix/Core/OS/RZWindow.h"

#include "Razix/Core/App/RZApplication.h"

#include "Razix/Core/RZEngineSettings.h"

namespace Razix {
    namespace Gfx {

        /* The Render API to use for rendering the application */
        enum RenderAPI
        {
            NONE = -1,
            VULKAN,    // MacOS/Linux
            D3D12,     // [WIP] // PC & XBOX
            GXM,       // Not Supported yet! (PSVita)
            GCM,       // Not Supported yet! (PS3)
            AGC,       // Not Supported yet! (PlayStation 5)
        };

        /* Global instance for Graphics Device level features */
        RAZIX_API extern GraphicsFeatures g_GraphicsFeatures;

        /**
         * The Graphics Context that manages the context of the underlying graphics API
         */
        class RAZIX_API RZGraphicsContext
        {
        public:
            /**
             * Creates the Graphics context with the underlying API
             * 
             * @param properties The window properties with which the window was created with
             * @param window The window handle to which the Graphics will be rendered to
             */
            static void Create(const WindowProperties& properties, RZWindow* window);
            /* Release the graphics context object and it's resources */
            static void Destroy();
            static void PostGraphicsContextInit();
            static void PreGraphicsContextDestroy();

            /* Initializes the underlying Graphics API for rendering */
            virtual void Init() = 0;
            //! Only here the Destroy method is in PascalCase rest of the API must maintain a camelCase destroy
            virtual void DestroyContext() = 0;
            /* Clears the screen with the given color */
            virtual void ClearWithColor(f32 r, f32 g, f32 b) = 0;

            virtual void Wait() {}

            /* Returns the pointer to the underlying graphics API */
            static RZGraphicsContext* GetContext();

            /* Gets the current render API being used by the engine to render */
            static RenderAPI GetRenderAPI() { return s_RenderAPI; }
            /* Set the render API to use to render */
            static void SetRenderAPI(RenderAPI api) { s_RenderAPI = api; }
            /* Gets the render API being used as a string */
            static const std::string GetRenderAPIString();

        protected:
            /* Pointer to the underlying graphics APi implementation */
            static RZGraphicsContext* s_Context;
            /* The render API selected to render the application */
            static RenderAPI s_RenderAPI;
        };
    }    // namespace Gfx
}    // namespace Razix
