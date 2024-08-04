#pragma once

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/FrameGraph/RZBlackboard.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"

// Passes Data
#include "Razix/Graphics/Passes/Data/GlobalData.h"

// Passes
#include "Razix/Graphics/Passes/RZBloomPass.h"
#include "Razix/Graphics/Passes/RZCSMPass.h"
#include "Razix/Graphics/Passes/RZColorGradingPass.h"
#include "Razix/Graphics/Passes/RZCompositionPass.h"
#include "Razix/Graphics/Passes/RZFXAAPass.h"
#include "Razix/Graphics/Passes/RZGBufferPass.h"
#include "Razix/Graphics/Passes/RZGIPass.h"
#include "Razix/Graphics/Passes/RZGaussianBlurPass.h"
#include "Razix/Graphics/Passes/RZHelloTrianglePass.h"
#include "Razix/Graphics/Passes/RZPBRDeferredLightingPass.h"
#include "Razix/Graphics/Passes/RZPBRLightingPass.h"
#include "Razix/Graphics/Passes/RZSSAOPass.h"
#include "Razix/Graphics/Passes/RZShadowPass.h"
#include "Razix/Graphics/Passes/RZSkyboxPass.h"
#include "Razix/Graphics/Passes/RZTAAResolvePass.h"
#include "Razix/Graphics/Passes/RZTonemapPass.h"

// Renderers
#include "Razix/Graphics/Renderers/RZImGuiRenderer.h"

#include "Razix/Maths/RZGrid.h"

#include "Razix/Graphics/Renderers/RZRendererSettings.h"

// TODO: [HiZ] https://miketuritzin.com/post/hierarchical-depth-buffers/
// TODO: [Random] https://www.jeremyong.com/cpp/2021/05/20/graphics-pipelines-for-young-bloods/
// TODO: [TAA] https://www.elopezr.com/temporal-aa-and-the-quest-for-the-holy-trail/
// TODO: [FXAA] https://blog.simonrodriguez.fr/articles/2016/07/implementing_fxaa.html

/*
    Format: For FXAA, you generally want to use a format that matches your final output format, which is typically an 8-bit per channel format like RGBA8. This is because FXAA operates on the LDR image after tone mapping.

    Texture Sampler Settings:

    Filtering: Use bilinear filtering. FXAA operates on the assumption that the texture is sampled with bilinear filtering.
    Wrap Mode: Typically, clamp-to-edge is used, as you usually don't want to wrap around the edges of the screen.

    ------------------------------------------------------------------------------------------------------------------------------------------------------------

    TAA works on the high dynamic range (HDR) image to leverage the additional color and brightness information available before it is compressed by the tone mapping process.

    As for the format and texture sampler settings for TAA:

    Format: For TAA, you usually want to use a floating-point format for the intermediate render targets to preserve the HDR information. Formats like RGBA16F or RGBA32F are commonly used.

    Texture Sampler Settings:

    Filtering: Use bilinear filtering. TAA relies on interpolating between pixels, so bilinear filtering is a good choice.
    Wrap Mode: Clamp-to-edge is typically used to avoid wrapping around the edges of the screen.
*/

namespace Razix {
    // Forward Declarations
    class RZScene;

    namespace Maths {
        class RZFrustum;
    }

    namespace Graphics {

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

            /* On Update for world renderer passes */
            void OnUpdate(RZTimestep dt);

            /* ImGui rendering for the world renderer */
            void OnImGui();

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
            RZShadowPass              m_ShadowPass;
            RZCSMPass                 m_CSMPass;
            RZGBufferPass             m_GBufferPass;
            RZSSAOPass                m_SSAOPass;
            RZPBRDeferredLightingPass m_PBRDeferredPass;
            RZPBRLightingPass         m_PBRLightingPass;
            RZSkyboxPass              m_SkyboxPass;
            RZGaussianBlurPass        m_GaussianBlurPass;
            RZImGuiRenderer           m_ImGuiRenderer;
            RZTAAResolvePass          m_TAAResolvePass;
            RZFXAAPass                m_FXAAPass;
            RZToneMapPass             m_TonemapPass;
            RZCompositionPass         m_CompositePass;
            RZHelloTrianglePass       m_HelloTrianglePass;

            //RZColorGradingPass        m_ColorGradingPass;

            // Other Variables
            u32         m_FrameCount                                            = 0;
            Maths::AABB m_SceneAABB                                             = {};
            glm::vec2   m_TAAJitterHaltonSamples[NUM_HALTON_SAMPLES_TAA_JITTER] = {};
            glm::mat4   m_PreviousViewProj                                      = {};
            glm::vec2   m_Jitter                                                = {};
            glm::vec2   m_PreviousJitter                                        = {};
            bool        m_FrameGraphBuildingInProgress                          = true;
            bool        m_IsFGFilePathDirty                                     = false;
            std::string m_FrameGraphFilePath                                    = "//RazixFG/Graphs/FrameGraph.Builtin.PBRLighting.json";
            //std::string m_FrameGraphFilePath           = "//RazixFG/Graphs/FrameGraph.User.EditorTest.json";

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
            void cullLights(Maths::RZFrustum& frustum);
            void uploadFrameData(RZScene* scene, RZRendererSettings& settings);
            void uploadLightsData(RZScene* scene, RZRendererSettings& settings);
        };
    }    // namespace Graphics
}    // namespace Razix
