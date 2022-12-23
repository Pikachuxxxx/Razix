#pragma once

#include "Razix/Graphics/API/RZRenderContext.h"

#include "Razix/Graphics/FrameGraph/RZBlackboard.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"

#include "Razix/Graphics/FrameGraph/Resources/RZTransientResources.h"

// Passes
#include "Razix/Graphics/Passes/RZFinalCompositionPass.h"

// Renderers
#include "Razix/Graphics/Renderers/RZImGuiRenderer.h"

namespace Razix {
    // Forward Declarations
    class RZScene;
    namespace Graphics {

        // Default pass data types
        struct RTOnlyPassData
        {
            FrameGraph::RZFrameGraphResource outputRT;          /* Render Texture to which this pass is drawn to                                                */
            FrameGraph::RZFrameGraphResource passDoneSemaphore; /* Semaphore to notify the command buffers that this pass has completed execution on the GPU    */
        };

        struct RTDTPassData
        {
            FrameGraph::RZFrameGraphResource outputRT;
            FrameGraph::RZFrameGraphResource depthRT;
            FrameGraph::RZFrameGraphResource passDoneSemaphore;
        };

        // Renderer Settings + Debug flags

        enum RZRendererFeatures : uint32_t
        {
            RendererFeature_None = 0,

            RendererFeature_Shadows  = 1 << 0,
            RendererFeature_GI       = 1 << 1,
            RendererFeature_IBL      = 1 << 2,
            RendererFeature_SSAO     = 1 << 3,
            RendererFeature_SSR      = 1 << 4,
            RendererFeature_Bloom    = 1 << 5,
            RendererFeature_FXAA     = 1 << 6,
            RendererFeature_Vignette = 1 << 7,
            RendererFeature_ImGui    = 1 << 8,

            RendererFeature_Default = RendererFeature_Shadows | RendererFeature_SSAO |
                                      RendererFeature_Bloom | RendererFeature_FXAA |
                                      RendererFeature_Vignette | RendererFeature_ImGui,

            RendererFeature_All = RendererFeature_Default | RendererFeature_SSR,
        };

        enum RZRendererDebugFlag : uint32_t
        {
            RendererDebugFlag_None          = 0,
            RendererDebugFlag_Wireframe     = 1 << 0,
            RendererDebugFlag_CascadeSplits = 1 << 1,
            RendererDebugFlag_VPL           = 1 << 2,
            RendererDebugFlag_RadianceOnly  = 1 << 3,
        };

        struct RZRendererSettings
        {
            // TODO: Specify the output RT type here
            uint32_t renderFeatures{RendererFeature_Default};
            struct
            {
                float radius{0.005f};
                float strength{0.04f};
            } bloom;
            struct
            {
                int32_t numPropagations{6};
            } globalIllumination;
            uint32_t debugFlags{0u};
            // TODO: Add tone mapping settings here
        };

        /**
         * Razix World Renderer handles rendering everything in the scene, it build and compiles and frame graph and is responsible for execution
         */
        class RAZIX_API RZWorldRenderer
        {
        public:
            RZWorldRenderer()  = default;
            ~RZWorldRenderer() = default;

            /**
             * Builds the frame graph using all the passes
             * 
             * Note: exports the svg & dot file into the Game/Exports/FrameGraph folder
             */
            void buildFrameGraph(RZRendererSettings settings, Razix::RZScene* scene);
            /* draws the frame by executing the frame graph */
            void drawFrame(RZRendererSettings settings, Razix::RZScene* scene);

            void destroy();

        private:
            FrameGraph::RZFrameGraph         m_FrameGraph;
            FrameGraph::RZBlackboard         m_Blackboard;
            FrameGraph::RZTransientResources m_TransientResources;
            // Frame Graph Import Data
            RZTexture2D* brdfLUTTexture;
            // List of all passes and data in the frame graph
            RZFinalCompositionPass m_CompositePass;
            // Renderers
            RZImGuiRenderer m_ImGuiRenderer;
        };
    }    // namespace Graphics
}    // namespace Razix
