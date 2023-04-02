#pragma once

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/FrameGraph/RZBlackboard.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"

#include "Razix/Graphics/FrameGraph/Resources/RZTransientResources.h"

// Passes
#include "Razix/Graphics/Passes/RZBloomPass.h"
#include "Razix/Graphics/Passes/RZDeferredLightingPass.h"
#include "Razix/Graphics/Passes/RZFinalCompositionPass.h"
#include "Razix/Graphics/Passes/RZGBufferPass.h"
#include "Razix/Graphics/Passes/RZGIPass.h"

#include "Razix/Graphics/Passes/Data/GlobalData.h"

// Renderers
#include "Razix/Graphics/Renderers/RZCascadedShadowsRenderer.h"
#include "Razix/Graphics/Renderers/RZForwardRenderer.h"
#include "Razix/Graphics/Renderers/RZImGuiRenderer.h"
#include "Razix/Graphics/Renderers/RZShadowRenderer.h"

#include "Razix/Maths/RZGrid.h"

namespace Razix {
    // Forward Declarations
    class RZScene;

    class RZCascadedShadowsRenderer;

    namespace Maths {
        class RZFrustum;
    }

    namespace Graphics {

        // Renderer Settings + Debug flags

        enum RZRendererFeatures : u32
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

        enum RZRendererDebugFlag : u32
        {
            RendererDebugFlag_None          = 0,
            RendererDebugFlag_Wireframe     = 1 << 0,
            RendererDebugFlag_CascadeSplits = 1 << 1,
            RendererDebugFlag_VPL           = 1 << 2,
            RendererDebugFlag_RadianceOnly  = 1 << 3,
        };

        enum TonemapMode : u32
        {
            ACES,
            Filmic,
            Lottes,
            Reinhard,
            Reinhard_V2,
            Uchimura,
            Uncharted2,
            Unreal,
            None
        };

        struct RZRendererSettings
        {
            // TODO: Specify the output RT type here
            u32 renderFeatures{RendererFeature_Default};
            struct
            {
                f32 radius{0.005f};
                f32 strength{0.04f};
            } bloomConfig;
            struct
            {
                int32_t numPropagations{6};
            } globalIlluminationConfig;
            u32         debugFlags{0u};
            TonemapMode tonemapMode = ACES;
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
            void buildFrameGraph(RZRendererSettings& settings, Razix::RZScene* scene);
            /* draws the frame by executing the frame graph */
            void drawFrame(RZRendererSettings& settings, Razix::RZScene* scene);
            /* Destroy frame graph passes and it's resources */
            void destroy();

            void importGlobalLightProbes(GlobalLightProbe globalLightProbe);
            void cullLights(Maths::RZFrustum& frustum);

        private:
            FrameGraph::RZFrameGraph         m_FrameGraph;
            FrameGraph::RZBlackboard         m_Blackboard;
            FrameGraph::RZTransientResources m_TransientResources;
            // Frame Graph Import Data
            RZTexture2D*     m_BRDFfLUTTexture = nullptr;
            RZCubeMap*       m_Skybox          = nullptr;
            GlobalLightProbe m_GlobalLightProbes{};
            // List of all passes, renderers and data in the frame graph
            RZCascadedShadowsRenderer m_CascadedShadowsRenderer;
            RZShadowRenderer          m_ShadowRenderer;
            RZGIPass                  m_GIPass;
            RZGBufferPass             m_GBufferPass;
            RZDeferredLightingPass    m_DeferredPass;
            RZImGuiRenderer           m_ImGuiRenderer;
            RZFinalCompositionPass    m_CompositePass;
            RZBloomPass               m_BloomPass;

            // Test only
            RZForwardRenderer m_ForwardRenderer;

            // Other Variables
            Maths::RZAABB m_SceneAABB;

        private:
            void uploadFrameData(RZScene* scene, RZRendererSettings settings);
        };
    }    // namespace Graphics
}    // namespace Razix
