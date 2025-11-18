#pragma once

#include "Razix/Core/Utils/RZTimestep.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RZGfxUtil.h"

#include "Razix/Gfx/FrameGraph/RZBlackboard.h"
#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"

// Passes Data
#include "Razix/Gfx/Passes/GlobalData.h"

// Passes
#include "Razix/Gfx/Passes/IRZPass.h"
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

#include "Razix/Gfx/Renderers/RZRendererSettings.h"

#include "Razix/Math/Grid.h"

// Debug Macro Settings

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

        struct RenderSyncPrimitives
        {
            // [Source]: https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html
            struct
            {
                // supports both timeline and fences in vulkan based on VK_KHR_timeline_semaphore availability
                rz_gfx_syncobj imageAcquired[RAZIX_MAX_FRAMES_IN_FLIGHT];     // one per frame in flight
                rz_gfx_syncobj renderingDone[RAZIX_MAX_SWAP_IMAGES_COUNT];    // one per swapchain image
            } presentSync = {};                                               // won't be needing this in DX12 and other APIs
            struct
            {
                u32            inFlightSyncIdx;    // current in-flight frame idx being recorded
                u32            _pad0[3];
                rz_gfx_syncobj inflightSyncobj[RAZIX_MAX_FRAMES_IN_FLIGHT];    // CPU->GPU sync, one per frame in flight (fence or timeline sema)
            } frameSync = {};
        };

        struct SamplersPool
        {
            rz_gfx_sampler_handle linearSampler;
            rz_gfx_sampler_handle pointSampler;
            rz_gfx_sampler_handle mipSampler;
            rz_gfx_sampler_handle anisotropicSampler;
            rz_gfx_sampler_handle shadowSampler;
            rz_gfx_sampler_handle shadowPCFSampler;
        };

        struct SamplersViewPool
        {
            rz_gfx_resource_view_handle linearSampler;
            rz_gfx_resource_view_handle pointSampler;
            rz_gfx_resource_view_handle mipSampler;
            rz_gfx_resource_view_handle anisotropicSampler;
            rz_gfx_resource_view_handle shadowSampler;
            rz_gfx_resource_view_handle shadowPCFSampler;
        };

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

            void buildFrameGraph(RZRendererSettings& settings, Razix::RZScene* scene);
            void drawFrame(RZRendererSettings& settings, Razix::RZScene* scene);

            void OnUpdate(RZTimestep dt);
            void OnImGui();
            void OnResize(u32 width, u32 height);
            void flushGPUWork();

            inline void clearFrameGraph() { m_FrameGraph.destroy(); }
            inline void pushRenderPass(IRZPass* pass, RZScene* scene, RZRendererSettings* settings) { pass->addPass(m_FrameGraph, scene, settings); }

            // Getters/Setters
            inline RZFrameGraph& getFrameGraph() { return m_FrameGraph; }
            inline RZString      getFrameGraphFilePath() const { return m_FrameGraphFilePath; }
            inline void          setFrameGraphFilePath(RZString val)
            {
                m_IsFGFilePathDirty  = true;
                m_FrameGraphFilePath = val;
            }
            inline void                           setReadbackSwapchainThisFrame() { m_ReadSwapchainThisFrame = true; }
            inline const rz_gfx_texture_readback* getSwapchainReadbackPtr() const { return &m_LastSwapchainReadback; }
            inline rz_gfx_cmdbuf_handle           getCurrCmdBufHandle() const { return m_InFlightDrawCmdBufHandles[m_RenderSync.frameSync.inFlightSyncIdx]; }
            inline const rz_gfx_texture*          getCurrSwapchainBackbufferPtr() const { return &m_Swapchain.backbuffers[m_Swapchain.currBackBufferIdx]; }
            inline rz_gfx_texture_handle          getCurrSwapchainBackbufferHandle() const { return m_Swapchain.backbuffers[m_Swapchain.currBackBufferIdx].resource.hot.handle; }
            inline const rz_gfx_resource_view*    getCurrSwapchainBackbufferResViewPtr() const { return &m_Swapchain.backbuffersResViews[m_Swapchain.currBackBufferIdx]; }
            inline rz_gfx_resource_view_handle    getCurrSwapchainBackbufferResViewHandle() const { return m_Swapchain.backbuffersResViews[m_Swapchain.currBackBufferIdx].resource.hot.handle; }
            inline rz_gfx_descriptor_heap_handle  getRenderTargetHeap() const { return m_RenderTargetHeap; }
            inline rz_gfx_descriptor_heap_handle  getDepthRenderTargetHeap() const { return m_DepthRenderTargetHeap; }
            inline rz_gfx_descriptor_heap_handle  getSamplerHeap() const { return m_SamplerHeap; }
            inline rz_gfx_descriptor_heap_handle  getResourceHeap() const { return m_ResourceHeap; }
            // TODO: Remove this, we will soon be using Immutable Samplers instead
            inline rz_gfx_descriptor_table_handle getGlobalSamplerTable() const { return m_GlobalSamplerTable; }

        private:
            static rz_gfx_descriptor_heap_handle m_RenderTargetHeap;
            static rz_gfx_descriptor_heap_handle m_DepthRenderTargetHeap;
            static rz_gfx_descriptor_heap_handle m_ResourceHeap;
            static rz_gfx_descriptor_heap_handle m_SamplerHeap;
            static SamplersPool                  m_SamplersPool;
            static SamplersViewPool              m_SamplersViewPool;
            RZFrameGraph                         m_FrameGraph                                            = {};
            rz_gfx_texture_handle                m_BRDFfLUTTextureHandle                                 = {};
            LightProbe                           m_GlobalLightProbes                                     = {};
            rz_gfx_texture_readback              m_LastSwapchainReadback                                 = {0};
            RZWindow*                            m_Window                                                = NULL;
            u32                                  m_FrameCount                                            = 0;
            bool                                 m_FrameGraphBuildingInProgress                          = false;
            bool                                 m_IsFGFilePathDirty                                     = false;
            bool                                 m_ReadSwapchainThisFrame                                = false;
            bool                                 _pad0                                                   = false;
            float2                               m_TAAJitterHaltonSamples[NUM_HALTON_SAMPLES_TAA_JITTER] = {};
            float4x4                             m_PreviousViewProj                                      = {};
            float2                               m_Jitter                                                = {};
            float2                               m_PreviousJitter                                        = {};
            RZString                             m_FrameGraphFilePath                                    = "//RazixFG/Graphs/FrameGraph.Builtin.PBRLighting.json";
            RenderSyncPrimitives                 m_RenderSync                                            = {};
            rz_gfx_swapchain                     m_Swapchain                                             = {};
            rz_gfx_cmdpool_handle                m_InFlightCmdPool[RAZIX_MAX_FRAMES_IN_FLIGHT]           = {};    // TODO: use a ring buffer for cmd buffers/pools/cpu syncobj and gpu syncobj per frame in flight per thread per submit
            rz_gfx_cmdbuf_handle                 m_InFlightDrawCmdBufHandles[RAZIX_MAX_FRAMES_IN_FLIGHT] = {};    // TODO: use a ring buffer for cmd buffers/pools/cpu syncobj and gpu syncobj per frame in flight per thread per submit
            const rz_gfx_cmdbuf*                 m_InFlightDrawCmdBufPtrs[RAZIX_MAX_FRAMES_IN_FLIGHT]    = {};    // TODO: use a ring buffer for cmd buffers/pools/cpu syncobj and gpu syncobj per frame in flight per thread per submit
            rz_gfx_resource_view_handle          m_FrameDataCBVHandle                                    = {};    // TODO: maintain them per in-flight frame
            rz_gfx_resource_view_handle          m_SceneLightsDataCBVHandle                              = {};    // TODO: maintain them per in-flight frame
            rz_gfx_descriptor_table_handle       m_GlobalSamplerTable                                    = {};    // TODO: maintain them per in-flight frame
            rz_gfx_descriptor_table_handle       m_FrameDataTable                                        = {};    // TODO: maintain them per in-flight frame
            rz_gfx_descriptor_table_handle       m_SceneLightsDataTable                                  = {};    // TODO: maintain them per in-flight frame
            u32                                  _pad1[2]                                                = {};
            rz_gfx_shader_handle                 m_ImGuiShaderHandle                                     = {};
            rz_gfx_root_signature_handle         m_ImGuiRootSigHandle                                    = {};
            rz_gfx_pipeline_handle               m_ImGuiPipelineHandle                                   = {};
            rz_gfx_descriptor_table_handle       m_ImGuiFontAtlasDescriptorSet                           = {};
            rz_gfx_buffer_handle                 m_ImGuiVB                                               = {};
            rz_gfx_buffer_handle                 m_ImGuiIB                                               = {};
            rz_gfx_texture_handle                m_ImGuiFontAtlasTexture                                 = {};
            rz_gfx_resource_view_handle          m_ImGuiFontSRVHandle                                    = {};

        private:
            //void importGlobalLightProbes(LightProbe globalLightProbe);
            //void cullLights(Maths::RZFrustum& frustum);
            //void uploadFrameData(RZScene* scene, RZRendererSettings& settings);
            //void uploadLightsData(RZScene* scene, RZRendererSettings& settings);
        };
    }    // namespace Gfx
}    // namespace Razix
