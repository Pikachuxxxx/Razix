#pragma once

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/RZCore.h"

#include "Razix/Core/OS/RZWindow.h"

#include "Razix/Core/App/RZApplication.h"

namespace Razix {
    namespace Gfx {

        /* The Render API to use for rendering the application */
        enum class RenderAPI
        {
            NONE   = -1,
            VULKAN = 1,    // Working
            D3D12  = 3,    // [WIP] // PC & XBOX
            GXM    = 4,    // Not Supported yet! (PSVita)
            GCM    = 5,    // Not Supported yet! (PS3)
            AGC,
            NX
        };

        // TODO: move this to RZEngineSettings, only run-time queried values live in this struct
        /**
         * Graphics Features as supported by the GPU, even though Engine supports them
         * the GPU can override certain setting and query run-time info like LaneWidth etc.
         */
        typedef struct GraphicsFeatures
        {
            bool EnableVSync                  = false; /* No V-Sync by default we don't cap the frame rate */
            bool TesselateTerrain             = true;
            bool SupportsBindless             = true;
            bool SupportsWaveIntrinsics       = false;
            bool SupportsShaderModel6         = false;
            bool SupportsNullIndexDescriptors = false;
            u32  MaxBindlessTextures          = 4096;
            u32  MinLaneWidth                 = 0;
            u32  MaxLaneWidth                 = 0;
        } GraphicsFeatures;

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
