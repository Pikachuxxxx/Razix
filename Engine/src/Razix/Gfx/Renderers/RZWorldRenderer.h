#pragma once

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Utilities/RZTimestep.h"

//#include "Razix/Gfx/FrameGraph/RZBlackboard.h"
//#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"

// Passes Data
//#include "Razix/Gfx/Passes/Data/GlobalData.h"

// Passes
//#include "Razix/Gfx/Passes/IRZPass.h"
//#include "Razix/Gfx/Passes/RZBloomPass.h"
//#include "Razix/Gfx/Passes/RZCSMPass.h"
//#include "Razix/Gfx/Passes/RZColorGradingPass.h"
//#include "Razix/Gfx/Passes/RZCompositionPass.h"
//#include "Razix/Gfx/Passes/RZFXAAPass.h"
//#include "Razix/Gfx/Passes/RZGBufferPass.h"
//#include "Razix/Gfx/Passes/RZGaussianBlurPass.h"
//#include "Razix/Gfx/Passes/RZPBRDeferredShadingPass.h"
//#include "Razix/Gfx/Passes/RZSSAOPass.h"
//#include "Razix/Gfx/Passes/RZShadowPass.h"
//#include "Razix/Gfx/Passes/RZSkyboxPass.h"
//#include "Razix/Gfx/Passes/RZTAAResolvePass.h"
//#include "Razix/Gfx/Passes/RZTonemapPass.h"
//#include "Razix/Gfx/Passes/RZVisibilityBufferFillPass.h"

// Renderers
#include "Razix/Gfx/Renderers/RZRendererSettings.h"

#include "Razix/Math/Grid.h"

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
    class RZWindow;

    namespace Maths {
        class RZFrustum;
    }

    namespace Gfx {

        /**
         * Razix World Renderer handles rendering everything in the scene, it build and compiles and frame graph and is responsible for execution
         * Also holds the Gfx resources and controls rendering flow
         */
        class RAZIX_API RZWorldRenderer
        {
        public:
            RZWorldRenderer()  = default;
            ~RZWorldRenderer() = default;

            void create(RZWindow* window, u32 width, u32 height);
            void destroy();

            /**
             * Builds the frame graph using all the passes
             * 
             * Note: exports the svg & dot file into the Game/Exports/FrameGraph folder
             */
            void buildFrameGraph(RZRendererSettings& settings, Razix::RZScene* scene);
            /* draws the frame by executing the frame graph */
            void drawFrame(RZRendererSettings& settings, Razix::RZScene* scene);

            /* On Update for world renderer passes */
            void OnUpdate(RZTimestep dt);

            /* ImGui rendering for the world renderer */
            void OnImGui();

            void OnResize(u32 width, u32 height) {}

            // Getters/Setters
            //inline RZFrameGraph& getFrameGraph() { return m_FrameGraph; }

            //inline std::string getFrameGraphFilePath() const { return m_FrameGraphFilePath; }
            //void               setFrameGraphFilePath(std::string val);
            //
            //inline void                   setReadbackSwapchainThisFrame() { m_ReadSwapchainThisFrame = true; }
            //inline const TextureReadback& getSwapchainReadback() { return m_LastSwapchainReadback; }
            //
            //void clearFrameGraph();
            //void pushRenderPass(IRZPass* pass, RZScene* scene, RZRendererSettings* settings);

        private:
            //RZFrameGraph    m_FrameGraph;
            //rz_texture_handle m_BRDFfLUTTextureHandle;
            //rz_texture_handle m_NoiseTextureHandle;
            //rz_texture_handle m_ColorGradingNeutralLUTHandle;
            //LightProbe      m_GlobalLightProbes;
            //RZShadowPass             m_ShadowPass;
            //RZGBufferPass            m_GBufferPass;
            //RZSSAOPass               m_SSAOPass;
            //RZPBRDeferredShadingPass m_PBRDeferredPass;
            //RZSkyboxPass             m_SkyboxPass;
            //RZToneMapPass            m_TonemapPass;
            //RZCompositionPass        m_CompositePass;
            //TextureReadback m_LastSwapchainReadback;
            RZWindow*   m_Window                                                = NULL;
            u32         m_FrameCount                                            = 0;
            float2      m_TAAJitterHaltonSamples[NUM_HALTON_SAMPLES_TAA_JITTER] = {};
            float4x4    m_PreviousViewProj                                      = {};
            float2      m_Jitter                                                = {};
            float2      m_PreviousJitter                                        = {};
            bool        m_FrameGraphBuildingInProgress                          = false;
            bool        m_IsFGFilePathDirty                                     = false;
            bool        m_ReadSwapchainThisFrame                                = false;
            std::string m_FrameGraphFilePath                                    = "//RazixFG/Graphs/FrameGraph.Builtin.PBRLighting.json";
            // Gfx resources
            struct RenderSyncPrimitives
            {
                struct
                {
                    rz_gfx_syncobj image_ready[RAZIX_MAX_SWAP_IMAGES_COUNT];
                    rz_gfx_syncobj rendering_done[RAZIX_MAX_SWAP_IMAGES_COUNT];

                } present_sync;    // won't be needing this in DX12
                union
                {
                    rz_gfx_syncobj inflightSyncobj[RAZIX_MAX_FRAMES_IN_FLIGHT];
                    struct
                    {
                        rz_gfx_syncobj   timelineSyncobj;
                        rz_gfx_timestamp frameTimestamps[RAZIX_MAX_FRAMES_IN_FLIGHT];
                        rz_gfx_timestamp globalTimestamp;
                        uint32_t         _pad[8];
                    };
                } frameSync;    // supports both timeline and fences in vulkan based on VK_KHR_timeline_semaphore availability
            } m_RenderSync;
            rz_gfx_swapchain m_Swapchain;

        private:
            //void importGlobalLightProbes(LightProbe globalLightProbe);
            //void cullLights(Maths::RZFrustum& frustum);
            //void uploadFrameData(RZScene* scene, RZRendererSettings& settings);
            //void uploadLightsData(RZScene* scene, RZRendererSettings& settings);
        };
    }    // namespace Gfx
}    // namespace Razix
