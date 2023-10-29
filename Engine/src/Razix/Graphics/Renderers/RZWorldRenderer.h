#pragma once

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/FrameGraph/RZBlackboard.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"

// Passes Data
#include "Razix/Graphics/Passes/Data/GlobalData.h"

// Passes
#include "Razix/Graphics/Passes/RZBloomPass.h"
#include "Razix/Graphics/Passes/RZColorGradingPass.h"
#include "Razix/Graphics/Passes/RZDeferredLightingPass.h"
#include "Razix/Graphics/Passes/RZFinalCompositionPass.h"
#include "Razix/Graphics/Passes/RZGBufferPass.h"
#include "Razix/Graphics/Passes/RZGIPass.h"
#include "Razix/Graphics/Passes/RZPBRLightingPass.h"
#include "Razix/Graphics/Passes/RZShadowPass.h"
#include "Razix/Graphics/Passes/RZSkyboxPass.h"

// Renderers
#include "Razix/Graphics/Renderers/RZCascadedShadowsRenderer.h"
#include "Razix/Graphics/Renderers/RZForwardRenderer.h"
#include "Razix/Graphics/Renderers/RZImGuiRenderer.h"

#include "Razix/Maths/RZGrid.h"

namespace Razix {
    // Forward Declarations
    class RZScene;
    class RZCascadedShadowsRenderer;

    namespace Maths {
        class Frustum;
    }

    namespace Graphics {

        // Renderer Settings + Debug flags
        enum RendererFeatures : u32
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

            RendererFeature_Default = RendererFeature_Shadows | RendererFeature_ImGui,

            RendererFeature_All = RendererFeature_Default | RendererFeature_SSR,
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
            TonemapMode tonemapMode         = ACES;
            bool        useProceduralSkybox = false;
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

            // Getters/Setters
            FrameGraph::RZFrameGraph& getFrameGraph() { return m_FrameGraph; }

            RAZIX_INLINE std::string getFrameGraphFilePath() const { return m_FrameGraphFilePath; }
            RAZIX_INLINE void        setFrameGraphFilePath(std::string val);

        private:
            FrameGraph::RZFrameGraph m_FrameGraph;
            // Frame Graph Import Data
            RZTextureHandle m_BRDFfLUTTextureHandle;
            RZTextureHandle m_NoiseTextureHandle;
            RZTextureHandle m_ColorGradingNeutralLUTHandle;
            LightProbe      m_GlobalLightProbes{};
            // List of all passes, renderers and data in the frame graph
            RZCascadedShadowsRenderer m_CascadedShadowsRenderer;
            RZShadowPass              m_ShadowPass;
            RZGIPass                  m_GIPass;
            RZGBufferPass             m_GBufferPass;
            RZDeferredLightingPass    m_DeferredPass;
            RZPBRLightingPass         m_PBRLightingPass;
            RZSkyboxPass              m_SkyboxPass;
            RZBloomPass               m_BloomPass;
            RZImGuiRenderer           m_ImGuiRenderer;
            RZColorGradingPass        m_ColorGradingPass;
            RZFinalCompositionPass    m_CompositePass;

            // Test only
            RZForwardRenderer m_ForwardRenderer;

            // Other Variables
            Maths::AABB m_SceneAABB;

            bool        m_FrameGraphBuildingInProgress = true;
            std::string m_FrameGraphFilePath           = "//RazixFG/Graphs/FrameGraph.Builtin.PBRLighting.json";
            //std::string m_FrameGraphFilePath           = "//RazixFG/Graphs/FrameGraph.User.EditorTest.json";
            bool m_IsFGFilePathDirty = false;

        private:
            /**
             * Imports the Global skybox, diffuse and specular light probes into the scene
             * 
             * @param globalLightProbe The global light probe that will be imported into the scene
             */
            void importGlobalLightProbes(LightProbe globalLightProbe);
            /**
             * Culls the scene lights against the Main Camera frustum
             */
            void cullLights(Maths::Frustum& frustum);
            void uploadFrameData(RZScene* scene, RZRendererSettings& settings);
            void uploadLightsData(RZScene* scene, RZRendererSettings& settings);
        };
    }    // namespace Graphics
}    // namespace Razix
