// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZWorldRenderer.h"

#define ENABLE_CODE_DRIVEN_FG_PASSES 0

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

//#include "Razix/Gfx/FrameGraph/RZBlackboard.h"
//#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"

//#include "Razix/Gfx/Lighting/RZImageBasedLightingProbesManager.h"

//#include "Razix/Gfx/Passes/Data/GlobalData.h"

//#include "Razix/Gfx/Renderers/RZImGuiRendererProxy.h"
//#include "Razix/Gfx/Renderers/RZDebugRendererProxy.h"

//#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
//#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Math/Grid.h"
#include "Razix/Math/ImportanceSampling.h"

//#include "Razix/Scene/Components/RZComponents.h"

//#include "Razix/Scene/RZScene.h"

#include "Razix/Tools/Runtime/RZEngineRuntimeTools.h"

#include "Razix/Utilities/RZColorUtilities.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3.h>
    #include <GLFW/glfw3native.h>
#endif

namespace Razix {
    namespace Gfx {

        /*
        static void ExportFrameGraphVisFile(const RZFrameGraph& framegraph)
        {
            auto        now   = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            std::tm     local_tm;
#if defined(_MSC_VER)    // For MSVC (Windows)
            localtime_s(&local_tm, &now_c);
#else    // For GCC/Clang (Linux/macOS)
            localtime_r(&now_c, &local_tm);
#endif
            std::ostringstream timestamp;
            timestamp << std::put_time(&local_tm, "%Y_%m_%d_%H_%M");

            std::string outPath;
            RZVirtualFileSystem::Get().resolvePhysicalPath("//RazixContent/FrameGraphs", outPath, true);

            // Construct the filename with the timestamp
            std::string filename = outPath + "/fg_debug_draw_test_" + timestamp.str() + ".dot";

            RAZIX_CORE_INFO("Exporting FrameGraph .... to ({0})", filename);

            std::ofstream os(filename);
            os << framegraph;
        }
        */

        //-------------------------------------------------------------------------------------------

        void RZWorldRenderer::create(RZWindow* window, u32 width, u32 height)
        {
            m_Window = window;
            memset(&m_RenderSync, 0, sizeof(RenderSyncPrimitives));

            // Create the swapchain
            GLFWwindow*   glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindow());
            rz_render_api api        = rzGfxCtx_GetRenderAPI();

#ifdef RAZIX_PLATFORM_WINDOWS
            if (api == RZ_RENDER_API_D3D12) {
                HWND hwnd = glfwGetWin32Window(glfwWindow);
                rzRHI_CreateSwapchain(&m_Swapchain, &hwnd, width, height);
            } else if (api == RZ_RENDER_API_VULKAN) {
                VkSurfaceKHR surface = VK_NULL_HANDLE;
                glfwCreateWindowSurface(g_GfxCtx.vk.instance, glfwWindow, nullptr, &surface);
                rzRHI_CreateSwapchain(&m_Swapchain, &surface, width, height);
            }

#elif defined(RAZIX_PLATFORM_MACOS) || defined(RAZIX_PLATFORM_LINUX)
            if (api == RZ_RENDER_API_VULKAN) {
                VkSurfaceKHR surface = VK_NULL_HANDLE;
                glfwCreateWindowSurface(g_GfxCtx.vk.instance, glfwWindow, nullptr, &surface);
                rzRHI_CreateSwapchain(&m_Swapchain, &surface, width, height);
            } else {
                RAZIX_ASSERT(false && "Only Vulkan is supported on this platform!");
            }

#else
    #error "Unsupported platform!"
#endif

            // create frame sync primitives
            if (g_GraphicsFeatures.support.SupportsTimelineSemaphores) {
                rzRHI_CreateSyncobj(&m_RenderSync.frameSync.timelineSyncobj, RZ_GFX_SYNCOBJ_TYPE_CPU);
            } else {
                for (u32 i = 0; i < RAZIX_MAX_FRAMES_IN_FLIGHT; i++) {
                    rzRHI_CreateSyncobj(&m_RenderSync.frameSync.inflightSyncobj[i], RZ_GFX_SYNCOBJ_TYPE_CPU);
                }
            }

            for (u32 i = 0; i < RAZIX_MAX_FRAMES_IN_FLIGHT; i++) {
                rz_gfx_cmdpool_desc cmdPoolDesc = {};
                cmdPoolDesc.poolType            = RZ_GFX_CMDPOOL_TYPE_GRAPHICS;
                m_InFlightCmdPool[i]            = RZResourceManager::Get().createCommandPool("InFlightCommandPool", cmdPoolDesc);

                rz_gfx_cmdbuf_desc desc        = {0};
                desc.pool                      = RZResourceManager::Get().getCommandPoolResource(m_InFlightCmdPool[i]);
                m_InFlightDrawCmdBufHandles[i] = RZResourceManager::Get().createCommandBuffer("InFlightDrawCommandBuffer", desc);
                m_InFlightDrawCmdBufPtrs[i]    = RZResourceManager::Get().getCommandBufferResource(m_InFlightDrawCmdBufHandles[i]);
            }
        }

        void RZWorldRenderer::destroy()
        {
            m_FrameCount = 0;

            //if (m_LastSwapchainReadback.data) {
            //    Memory::RZFree(m_LastSwapchainReadback.data);O.
            //    m_LastSwapchainReadback.data = NULL;
            //}

            // Wait for rendering to be done before halting
            rzRHI_FlushGPUWork(&m_RenderSync.frameSync.timelineSyncobj, &m_RenderSync.frameSync.globalTimestamp);

            m_FrameGraphBuildingInProgress = true;

#if RX_ENABLE_GFX
            // Destroy Frame Graph Transient Resources
            m_FrameGraph.destroy();

            // Destroy Renderers
            RZImGuiRendererProxy::Get().Destroy();
            RZDebugRendererProxy::Get().Destroy();

            // Destroy Passes
            m_ShadowPass.destroy();
            m_GBufferPass.destroy();
            m_PBRDeferredPass.destroy();
            m_SkyboxPass.destroy();
            m_CompositePass.destroy();
#endif
            for (u32 i = 0; i < RAZIX_MAX_FRAMES_IN_FLIGHT; i++) {
                RZResourceManager::Get().destroyCommandPool(m_InFlightCmdPool[i]);
                RZResourceManager::Get().destroyCommandBuffer(m_InFlightDrawCmdBufHandles[i]);
            }

            if (g_GraphicsFeatures.support.SupportsTimelineSemaphores) {
                rzRHI_DestroySyncobj(&m_RenderSync.frameSync.timelineSyncobj);
            } else {
                for (u32 i = 0; i < RAZIX_MAX_FRAMES_IN_FLIGHT; i++) {
                    rzRHI_DestroySyncobj(&m_RenderSync.frameSync.inflightSyncobj[i]);
                }
            }

            rzRHI_DestroySwapchain(&m_Swapchain);
        }

        /**
         * Notes:
         * 1. In Razix we use CW winding order for front facing triangles => Also, back facing faces are pointed towards the camera
         */

        void RZWorldRenderer::buildFrameGraph(RZRendererSettings& settings, Razix::RZScene* scene)
        {
#if 0
            memset(&m_LastSwapchainReadback, 0, sizeof(TextureReadback));

            m_FrameGraphBuildingInProgress = true;

            // Noise texture LUT
            RZTextureDesc noiseDesc     = {};
            noiseDesc.name              = "VolumetricCloudsNoise";    // must match shader
            noiseDesc.enableMips        = false;
            noiseDesc.filePath          = "//RazixContent/Textures/Texture.Builtin.VolumetricCloudsNoise.png";
            m_NoiseTextureHandle        = RZResourceManager::Get().createTexture(noiseDesc);
            auto& volumetricData        = m_FrameGraph.getBlackboard().add<VolumetricCloudsData>();
            volumetricData.noiseTexture = m_FrameGraph.import <RZFrameGraphTexture>(noiseDesc.name, CAST_TO_FG_TEX_DESC noiseDesc, {m_NoiseTextureHandle});

            // Import the color grading LUT
            RZTextureDesc colorGradingNeutralLUTDesc = {};
            colorGradingNeutralLUTDesc.name          = "ColorGradingUnreal_Neutral_LUT16";    // must match shader
            colorGradingNeutralLUTDesc.enableMips    = false;
            colorGradingNeutralLUTDesc.flipY         = true;
            colorGradingNeutralLUTDesc.filePath      = "//RazixContent/Textures/Texture.Builtin.ColorGradingNeutralLUT16.png";
            m_ColorGradingNeutralLUTHandle           = RZResourceManager::Get().createTexture(colorGradingNeutralLUTDesc);
            auto& colorGradingLUTSData               = m_FrameGraph.getBlackboard().add<ColorGradingLUTData>();
            colorGradingLUTSData.neutralLUT          = m_FrameGraph.import <RZFrameGraphTexture>(colorGradingNeutralLUTDesc.name, CAST_TO_FG_TEX_DESC colorGradingNeutralLUTDesc, {m_ColorGradingNeutralLUTHandle});

            // Upload buffers/textures Data to the FrameGraph and GPU initially
            // Upload BRDF look up texture to the GPU
            RZTextureDesc brdfDesc  = {};
            brdfDesc.name           = "BrdfLUT";    // must match shader
            brdfDesc.enableMips     = false;
            brdfDesc.filePath       = "//RazixContent/Textures/Texture.Builtin.BrdfLUT.png";
            m_BRDFfLUTTextureHandle = RZResourceManager::Get().createTexture(brdfDesc);
            auto& brdfData          = m_FrameGraph.getBlackboard().add<BRDFData>();
            brdfData.lut            = m_FrameGraph.import <RZFrameGraphTexture>(brdfDesc.name, CAST_TO_FG_TEX_DESC brdfDesc, {m_BRDFfLUTTextureHandle});

            //-----------------------------------------------------------------------------------

            // Load the Skybox and Global Light Probes
            // FIXME: This is hard coded make this a user land material
            m_GlobalLightProbes.skybox   = RZImageBasedLightingProbesManager::convertEquirectangularToCubemap("//RazixContent/Textures/HDR/teufelsberg_inner_4k.hdr");
            m_GlobalLightProbes.diffuse  = RZImageBasedLightingProbesManager::generateIrradianceMap(m_GlobalLightProbes.skybox);
            m_GlobalLightProbes.specular = RZImageBasedLightingProbesManager::generatePreFilteredMap(m_GlobalLightProbes.skybox);
            // Import this into the Frame Graph
            importGlobalLightProbes(m_GlobalLightProbes);

            //-----------------------------------------------------------------------------------
            // Misc Variables

            // Jitter samples for TAA
            for (int i = 0; i < NUM_HALTON_SAMPLES_TAA_JITTER; ++i) {
                // Generate jitter using Halton sequence with bases 2 and 3 for X and Y respectively
                m_TAAJitterHaltonSamples[i].x = 2.0f * (f32) (Math::ImportanceSampling::HaltonSequenceSample(i + 1, 2) - 1.0f);    // Centering the jitter around (0,0)
                m_TAAJitterHaltonSamples[i].y = 2.0f * (f32) (Math::ImportanceSampling::HaltonSequenceSample(i + 1, 3) - 1.0f);    // Centering the jitter around (0,0)
                m_TAAJitterHaltonSamples[i].x /= RZApplication::Get().getWindow()->getWidth();
                m_TAAJitterHaltonSamples[i].y /= RZApplication::Get().getWindow()->getHeight();
            }

            //-----------------------------------------------------------------------------------

            // These are system level code passes so always enabled
            uploadFrameData(scene, settings);
            uploadLightsData(scene, settings);

            auto& frameDataBlock = m_FrameGraph.getBlackboard().get<FrameData>();

            //-----------------------------------------------------------------------------------

    #if ENABLE_DATA_DRIVEN_FG_PASSES
            //-------------------------------
            // Data Driven Frame Graph
            //-------------------------------

            if (!getFrameGraphFilePath().empty())
                RAZIX_ASSERT(m_FrameGraph.parse(getFrameGraphFilePath()), "[Frame Graph] Failed to parse graph!");
    #endif

            //-------------------------------
            // Simple Shadow map Pass
            //-------------------------------
            m_ShadowPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // GBuffer Pass
            //-------------------------------
            m_GBufferPass.addPass(m_FrameGraph, scene, &settings);
            auto& gBufferData = m_FrameGraph.getBlackboard().get<GBufferData>();

            //-------------------------------
            // PBR Deferred Pass
            //-------------------------------
            m_PBRDeferredPass.addPass(m_FrameGraph, scene, &settings);
            auto& sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            //-------------------------------
            // Skybox Pass
            //-------------------------------
            m_SkyboxPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // Tonemap Pass
            //-------------------------------
            //m_TonemapPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // Debug Scene Pass
            //-------------------------------
            m_FrameGraph.getBlackboard().add<DebugPassData>() = m_FrameGraph.addCallbackPass<DebugPassData>(
                "Pass.Builtin.Code.DebugDraw",
                [&](DebugPassData& data, RZPassResourceBuilder& builder) {
                    builder
                        .setAsStandAlonePass()
                        .setDepartment(Department::Debug);

                    RZDebugRendererProxy::Get().Init();

                    builder.read(frameDataBlock.frameData);

                    sceneData.SceneHDR = builder.write(sceneData.SceneHDR);
                    data.DebugRT       = sceneData.SceneHDR;

                    gBufferData.GBufferDepth = builder.write(gBufferData.GBufferDepth);
                    data.DebugDRT            = gBufferData.GBufferDepth;
                },
                [=](const DebugPassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("DebugDraw Pass");

                    // Origin point
                    //RZDebugRendererProxy::DrawPoint(float3(0.0f), 0.25f);

                    // X, Y, Z lines
                    RZDebugRendererProxy::DrawLine(float3(-100.0f, 0.0f, 0.0f), float3(100.0f, 0.0f, 0.0f), float4(1.0f, 0.0f, 0.0f, 1.0f));
                    RZDebugRendererProxy::DrawLine(float3(0.0f, -100.0f, 0.0f), float3(0.0f, 100.0f, 0.0f), float4(0.0f, 1.0f, 0.0f, 1.0f));
                    RZDebugRendererProxy::DrawLine(float3(0.0f, 0.0f, -100.0f), float3(0.0f, 0.0f, 100.0f), float4(0.0f, 0.0f, 1.0f, 1.0f));

                    // Grid
                    RZDebugRendererProxy::DrawGrid(125, float4(0.75f));

                    // Draw all lights in the scene
                    auto lights = scene->GetComponentsOfType<LightComponent>();
                    // Draw predefined light matrix
                    // Use the first directional light and currently only one Dir Light casts shadows, multiple just won't do anything in the scene not even light contribution
                    RZLight dir_light;
                    for (auto& light: lights) {
                        if (light.light.getType() == LightType::DIRECTIONAL) {
                            dir_light = light.light;
                            break;
                        }
                    }

                    float4x4 lightView  = lookAt(dir_light.getPosition(), float3(0.0f), float3(0.0f, 1.0f, 0.0f));
                    float    near_plane = -50.0f, far_plane = 50.0f;
                    float4x4 lightProjection = ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
                    lightProjection[1][1] *= -1;
                    auto lightViewProj = lightProjection * lightView;
                    RZDebugRendererProxy::DrawFrustum(lightViewProj, float4(0.863f, 0.28f, 0.21f, 1.0f));

                    // Draw all camera frustums
                    auto cameras = scene->GetComponentsOfType<CameraComponent>();
                    for (auto& camComponents: cameras) {
                        RZDebugRendererProxy::DrawFrustum(camComponents.Camera.getFrustum(), float4(0.2f, 0.85f, 0.1f, 1.0f));
                    }

                    // Draw AABBs for all the Meshes in the Scene
                    auto mesh_group = scene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
                    for (auto entity: mesh_group) {
                        // Draw the mesh renderer components
                        const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                        // Bind push constants, VBO, IBO and draw
                        float4x4 transform = mesh_trans.GetGlobalTransform();

                        if (mrc.Mesh && mrc.enableBoundingBoxes)
                            RZDebugRendererProxy::DrawAABB(mrc.Mesh->getBoundingBox().transform(transform), float4(0.0f, 1.0f, 0.0f, 1.0f));
                    }

                    RZDebugRendererProxy::Get().Begin(scene);

                    auto rt = resources.get<RZFrameGraphTexture>(data.DebugRT).getHandle();
                    auto dt = resources.get<RZFrameGraphTexture>(data.DebugDRT).getHandle();

                    RenderingInfo info    = {};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{rt, {false, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {dt, {false, ClearColorPresets::DepthOneToZero}};

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RHI::BeginRendering(cmdBuffer, info);

                    RZDebugRendererProxy::Get().Draw(cmdBuffer);

                    RHI::EndRendering(cmdBuffer);

                    RZDebugRendererProxy::Get().End();
                    RAZIX_TIME_STAMP_END();
                });

            //-------------------------------
            // ImGui Pass
            //-------------------------------

            m_FrameGraph.getBlackboard().add<ImGuiPassData>() = m_FrameGraph.addCallbackPass<ImGuiPassData>(
                "Pass.Builtin.Code.ImGui",
                [&](ImGuiPassData& data, RZPassResourceBuilder& builder) {
                    builder
                        .setAsStandAlonePass()
                        .setDepartment(Department::UI);

                    sceneData.SceneHDR = builder.write(sceneData.SceneHDR);
                    data.ImGuiRT       = sceneData.SceneHDR;

                    gBufferData.GBufferDepth = builder.write(gBufferData.GBufferDepth);
                    data.ImGuiDRT            = gBufferData.GBufferDepth;

                    RZImGuiRendererProxy::Get().Init();
                },
                [=](const ImGuiPassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("ImGui Pass");

                    RZImGuiRendererProxy::Get().Begin(scene);

                    auto rt = resources.get<RZFrameGraphTexture>(data.ImGuiRT).getHandle();
                    auto dt = resources.get<RZFrameGraphTexture>(data.ImGuiDRT).getHandle();

                    RenderingInfo info    = {};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{rt, {false, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {dt, {false, ClearColorPresets::DepthOneToZero}};

                    RHI::BeginRendering(Gfx::RHI::GetCurrentCommandBuffer(), info);

                    if (settings.renderFeatures & RendererFeature_ImGui)
                        RZImGuiRendererProxy::Get().Draw(Gfx::RHI::GetCurrentCommandBuffer());

                    RZImGuiRendererProxy::Get().End();
                    RAZIX_TIME_STAMP_END();
                });

            sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            //-------------------------------
            // Composition Pass
            //-------------------------------
            m_FrameGraph.getBlackboard().setFinalOutputName("SceneHDR");
            m_CompositePass.addPass(m_FrameGraph, scene, &settings);

            // Compile the Frame Graph
            RAZIX_CORE_INFO("Compiling FrameGraph....");
            m_FrameGraph.compile();

    #ifndef RAZIX_GOLD_MASTER
            // Dump the Frame Graph for visualization
            // NOTE: Careful this won't write to the Engine directory this is inside bin and build artifact
            // FIXME: Find a way to map VFS to OG Engine path pre-copy or idk just umm...be careful I guess
            ExportFrameGraphVisFile(m_FrameGraph);
    #endif

            m_FrameGraphBuildingInProgress = false;
#endif
        }

        void RZWorldRenderer::drawFrame(RZRendererSettings& settings, Razix::RZScene* scene)
        {
            m_FrameCount++;

            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            //if (m_IsFGFilePathDirty) {
            //    destroy();
            //    RZFrameGraph::ResetFirstFrame();
            //    buildFrameGraph(settings, RZSceneManager::Get().getCurrentScene());
            //    m_IsFGFilePathDirty = false;
            //}

            if (m_FrameGraphBuildingInProgress)
                return;

            // Main Frame Graph World Rendering Loop
            {
                // Acquire Image to render onto
                if (g_GraphicsFeatures.support.SupportsTimelineSemaphores)
                    rzRHI_BeginFrame(&m_Swapchain, &m_RenderSync.frameSync.timelineSyncobj, m_RenderSync.frameSync.frameTimestamps, &m_RenderSync.frameSync.globalTimestamp);

                // In DirectX 12, the swapchain back buffer index currBackBufferIdx directly maps to the index of the image
                // that is being presented/rendered to. This is because DXGI explicitly exposes the current back buffer index
                // via IDXGISwapChain::GetCurrentBackBufferIndex(), and the driver guarantees image acquisition in strict
                // presentation order (FIFO-like). As a result, synchronization objects like fences, command allocators, or
                // timestamp slots are usually tracked and reused per back buffer index.
                //
                // In contrast, Vulkan allows more flexibility: the acquired image index from vkAcquireNextImageKHR may return
                // any image in the swapchain (not necessarily in FIFO order). This requires applications to track resource
                // usage and in-flight sync per acquired image, using a round-robin or per-image tracking model. Since Vulkan
                // makes no guarantee about reuse pattern, it's incorrect to assume back buffer N will always follow N-1, hence
                // a more generalized ring buffer or semaphore timeline sync tracking model is required.
                // - DX12: Buffer index is reliable for indexing per-frame resources tracked per currBackBufferIdx.
                // - Vulkan: Image acquisition is non-linear; sync and frame data must be tracked per imageIndex returned from vkAcquireNextImageKHR.
                if (g_RenderAPI == RZ_RENDER_API_D3D12) {
                    m_RenderSync.frameSync.inFlightSyncIdx = m_Swapchain.currBackBufferIdx;
                }

                // Begin Recording  onto the command buffer, select one as per the frame idx
                const rz_gfx_cmdbuf_handle cmdBuffer = m_InFlightDrawCmdBufHandles[m_RenderSync.frameSync.inFlightSyncIdx];
                rzRHI_BeginCmdBuf(cmdBuffer);

                // Begin Frame Marker
                RAZIX_MARK_BEGIN("Frame # " + std::to_string(m_FrameCount) + " [back buffer # " + std::to_string(m_RenderSync.frameSync.inFlightSyncIdx) + " ]", float4(1.0f, 0.0f, 1.0f, 1.0f));

                RAZIX_CORE_INFO("curr rtv cpu ptr: {0}", (u32) m_Swapchain.backbuffers[m_Swapchain.currBackBufferIdx].dx12.resView.rtv.cpu.ptr);

                rzRHI_InsertSwapchainImageBarrier(cmdBuffer, &m_Swapchain.backbuffers[m_Swapchain.currBackBufferIdx], RZ_GFX_RESOURCE_STATE_PRESENT, RZ_GFX_RESOURCE_STATE_RENDER_TARGET);

                // Execute the Frame Graph passes
                m_FrameGraph.execute();

                rzRHI_InsertSwapchainImageBarrier(cmdBuffer, &m_Swapchain.backbuffers[m_Swapchain.currBackBufferIdx], RZ_GFX_RESOURCE_STATE_RENDER_TARGET, RZ_GFX_RESOURCE_STATE_PRESENT);

                // End Frame Marker
                RAZIX_MARK_END();

                // End command buffer recording
                rzRHI_EndCmdBuf(cmdBuffer);
                // Submit the render queue before presenting next
                rzRHI_SubmitCmdBuf(cmdBuffer);

                // swapchain capture is done before presentation
                if (m_ReadSwapchainThisFrame) {
                    m_ReadSwapchainThisFrame = false;

                    // Wait for rendering to be done before capturing
                    rzRHI_FlushGPUWork(&m_RenderSync.frameSync.timelineSyncobj, &m_RenderSync.frameSync.globalTimestamp);

                    // Use a single time command buffer to this
                    //RZDrawCommandBufferHandle cmdBuff = RZDrawCommandBuffer::BeginSingleTimeCommandBuffer("Swapchain Capture", Utilities::GenerateHashedColor4(24u));
                    //
                    //m_LastSwapchainReadback = RHI::InsertTextureReadback(cmdBuff, RHI::GetSwapchain()->GetCurrentBackBufferImage());
                    //
                    //RZDrawCommandBuffer::EndSingleTimeCommandBuffer(cmdBuff);
                }

                // Present the image to presentation engine as soon as rendering to COLOR_ATTACHMENT is done
                rzRHI_EndFrame(&m_Swapchain, &m_RenderSync.frameSync.timelineSyncobj, m_RenderSync.frameSync.frameTimestamps, &m_RenderSync.frameSync.globalTimestamp);

                m_RenderSync.present_sync.currSyncpointIdx = (m_RenderSync.present_sync.currSyncpointIdx + 1) % RAZIX_MAX_SWAP_IMAGES_COUNT;
                m_RenderSync.frameSync.inFlightSyncIdx     = (m_RenderSync.frameSync.inFlightSyncIdx + 1) % RAZIX_MAX_FRAMES_IN_FLIGHT;
            }
        }

        void RZWorldRenderer::OnUpdate(RZTimestep dt)
        {
        }

        void RZWorldRenderer::OnImGui()
        {
#ifndef RAZIX_GOLD_MASTER

            // TESTING IMGUI RUNTIME TOOLS CONFIG!
            // This will be owned by RZEngine
            static Tools::ToolsDrawConfig drawConfig = {};

            //Tools::OnImGuiDrawEngineTools(drawConfig);
#endif
        }

        void RZWorldRenderer::OnResize(u32 width, u32 height)
        {
            rzRHI_FlushGPUWork(&m_RenderSync.frameSync.timelineSyncobj, &m_RenderSync.frameSync.globalTimestamp);

            // m_FrameGraph.resize(width, height);
            rzRHI_ResizeSwapchain(&m_Swapchain, width, height);
            m_FrameGraph.resize(width, height);

            rzRHI_FlushGPUWork(&m_RenderSync.frameSync.timelineSyncobj, &m_RenderSync.frameSync.globalTimestamp);
        }

        RAZIX_INLINE void RZWorldRenderer::setFrameGraphFilePath(std::string val)
        {
            m_IsFGFilePathDirty  = true;
            m_FrameGraphFilePath = val;
        }

        void RZWorldRenderer::clearFrameGraph()
        {
            m_FrameGraph.destroy();
        }

        void RZWorldRenderer::pushRenderPass(IRZPass* pass, RZScene* scene, RZRendererSettings* settings)
        {
            pass->addPass(m_FrameGraph, scene, settings);
        }

#if 0
        //--------------------------------------------------------------------------

        void RZWorldRenderer::importGlobalLightProbes(LightProbe globalLightProbe)
        {
            auto& globalLightProbeData = m_FrameGraph.getBlackboard().add<GlobalLightProbeData>();

            auto SkyboxDesc   = RZResourceManager::Get().getPool<RZTexture>().get(globalLightProbe.skybox)->getDescription();
            auto DiffuseDesc  = RZResourceManager::Get().getPool<RZTexture>().get(globalLightProbe.diffuse)->getDescription();
            auto SpecularDesc = RZResourceManager::Get().getPool<RZTexture>().get(globalLightProbe.specular)->getDescription();

            SkyboxDesc.name   = "EnvironmentMap";
            DiffuseDesc.name  = "IrradianceMap";
            SpecularDesc.name = "PreFilteredMap";

            globalLightProbeData.environmentMap         = m_FrameGraph.import <RZFrameGraphTexture>("EnvironmentMap", CAST_TO_FG_TEX_DESC SkyboxDesc, {globalLightProbe.skybox});
            globalLightProbeData.diffuseIrradianceMap   = m_FrameGraph.import <RZFrameGraphTexture>("IrradianceMap", CAST_TO_FG_TEX_DESC DiffuseDesc, {globalLightProbe.diffuse});
            globalLightProbeData.specularPreFilteredMap = m_FrameGraph.import <RZFrameGraphTexture>("PreFilteredMap", CAST_TO_FG_TEX_DESC SpecularDesc, {globalLightProbe.specular});
        }

        //--------------------------------------------------------------------------

        void RZWorldRenderer::uploadFrameData(RZScene* scene, RZRendererSettings& settings)
        {
            m_FrameGraph.getBlackboard().add<FrameData>() = m_FrameGraph.addCallbackPass<FrameData>(
                "Pass.Builtin.Code.FrameDataUpload",
                [&](FrameData& data, RZPassResourceBuilder& builder) {
                    builder
                        .setAsStandAlonePass()
                        .setDepartment(Department::Core);

                    RZBufferDesc framedataBufferDesc          = {};
                    framedataBufferDesc.name                  = "FrameData";
                    framedataBufferDesc.size                  = sizeof(GPUFrameData);
                    framedataBufferDesc.data                  = NULL;
                    framedataBufferDesc.initResourceViewHints = kCBV;
                    framedataBufferDesc.usage                 = BufferUsage::PersistentStream;
                    data.frameData                            = builder.create<RZFrameGraphBuffer>(framedataBufferDesc.name, CAST_TO_FG_BUF_DESC framedataBufferDesc);

                    data.frameData = builder.write(data.frameData);
                },
                [=](const FrameData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("Upload FrameData");
                    RAZIX_MARK_BEGIN("Upload FrameData", float4(0.8f, 0.2f, 0.15f, 1.0f));

                    GPUFrameData gpuData{};
                    gpuData.time += gpuData.deltaTime;
                    gpuData.deltaTime      = RZEngine::Get().GetStatistics().DeltaTime;
                    gpuData.resolution     = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    gpuData.renderFeatures = settings.renderFeatures;

                    m_Jitter = m_TAAJitterHaltonSamples[(m_FrameCount % NUM_HALTON_SAMPLES_TAA_JITTER)];
                    // Based on scene sampling pattern set the apt jitter
                    if (RZEngine::Get().getWorldSettings().samplingPattern == Halton)
                        gpuData.jitterTAA = m_Jitter;

                    gpuData.previousJitterTAA = m_PreviousJitter;

                    auto& sceneCam = scene->getSceneCamera();

                    sceneCam.setAspectRatio(f32(RZApplication::Get().getWindow()->getWidth()) / f32(RZApplication::Get().getWindow()->getHeight()));

                    // clang-format off
                    float4x4 jitterMatrix = float4x4(
                        1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        gpuData.jitterTAA.x, gpuData.jitterTAA.y, 0.0, 1.0    // translation
                    );
                    // clang-format on

                    //auto jitteredProjMatrix = sceneCam.getProjection() * jitterMatrix;

                    gpuData.camera.projection         = sceneCam.getProjection();
                    gpuData.camera.inversedProjection = inverse(gpuData.camera.projection);
                    gpuData.camera.view               = sceneCam.getViewMatrix();
                    gpuData.camera.inversedView       = inverse(gpuData.camera.view);
                    gpuData.camera.prevViewProj       = m_PreviousViewProj;
                    gpuData.camera.fov                = sceneCam.getPerspectiveVerticalFOV();
                    gpuData.camera.nearPlane          = sceneCam.getPerspectiveNearClip();
                    gpuData.camera.farPlane           = sceneCam.getPerspectiveFarClip();

                    // update and upload the UBO
                    auto frameDataBufferHandle = resources.get<RZFrameGraphBuffer>(data.frameData).getHandle();
                    RZResourceManager::Get().getUniformBufferResource(frameDataBufferHandle)->SetData(sizeof(GPUFrameData), &gpuData);

                    // Since upload is done update the variables to store the previous data
                    {
                        m_PreviousJitter   = m_Jitter;
                        m_PreviousViewProj = gpuData.camera.projection * gpuData.camera.view;
                    }

                    if (!Gfx::RHI::Get().getFrameDataSet().isValid()) {
                        RZDescriptor descriptor                 = {};
                        descriptor.name                         = "Descriptor.FrameDataUBO";
                        descriptor.bindingInfo.location.binding = 0;
                        descriptor.bindingInfo.type             = DescriptorType::kUniformBuffer;
                        descriptor.bindingInfo.stage            = ShaderStage(ShaderStage::kVertex);
                        descriptor.uniformBuffer                = frameDataBufferHandle;

                        RZDescriptorSetDesc setCreateDesc = {};
                        setCreateDesc.name                = "DescriptorSet.GlobalFrameData";
                        setCreateDesc.heapType            = DescriptorHeapType::kCbvUavSrvHeap;
                        setCreateDesc.descriptors.push_back(descriptor);
                        setCreateDesc.setIdx = BindingTable_System::SET_IDX_FRAME_DATA;

                        auto m_FrameDataSet = RZResourceManager::Get().createDescriptorSet(setCreateDesc);
                        Gfx::RHI::Get().setFrameDataSet(m_FrameDataSet);
                    }
                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZWorldRenderer::uploadLightsData(RZScene* scene, RZRendererSettings& settings)
        {
            m_FrameGraph.getBlackboard().add<SceneLightsData>() = m_FrameGraph.addCallbackPass<SceneLightsData>(
                "Pass.Builtin.Code.SceneLightsDataUpload",
                [&](SceneLightsData& data, RZPassResourceBuilder& builder) {
                    builder
                        .setAsStandAlonePass()
                        .setDepartment(Department::Core);

                    RZBufferDesc lightdataBufferDesc          = {};
                    lightdataBufferDesc.name                  = "SceneLightsData";
                    lightdataBufferDesc.size                  = sizeof(GPULightsData);
                    lightdataBufferDesc.data                  = NULL;
                    lightdataBufferDesc.initResourceViewHints = kCBV;
                    lightdataBufferDesc.usage                 = BufferUsage::PersistentStream;
                    data.lightsDataBuffer                     = builder.create<RZFrameGraphBuffer>(lightdataBufferDesc.name, CAST_TO_FG_BUF_DESC lightdataBufferDesc);
                    data.lightsDataBuffer                     = builder.write(data.lightsDataBuffer);
                },
                [=](const SceneLightsData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("Upload SceneLights");
                    RAZIX_MARK_BEGIN("Upload SceneLights", float4(0.2f, 0.2f, 0.75f, 1.0f));

                    GPULightsData gpuLightsData{};

                    // Upload the lights data after updating some stuff such as position etc.
                    auto group = scene->getRegistry().group<LightComponent>(entt::get<TransformComponent>);
                    for (auto entity: group) {
                        const auto& [lightComponent, transformComponent] = group.get<LightComponent, TransformComponent>(entity);

                        lightComponent.light.getLightData().position = transformComponent.Translation;
                        lightComponent.light.setDirection(lightComponent.light.getLightData().position);
                        gpuLightsData.lightData[gpuLightsData.numLights] = lightComponent.light.getLightData();

                        gpuLightsData.numLights++;
                    }
                    // update and upload the UBO
                    auto lightsDataBuffer = resources.get<RZFrameGraphBuffer>(data.lightsDataBuffer).getHandle();
                    RZResourceManager::Get().getUniformBufferResource(lightsDataBuffer)->SetData(sizeof(GPULightsData), &gpuLightsData);

                    if (!Gfx::RHI::Get().getSceneLightsDataSet().isValid()) {
                        RZDescriptor descriptor                 = {};
                        descriptor.name                         = "Descriptor.SceneLightsDataUBO";
                        descriptor.bindingInfo.location.binding = 0;
                        descriptor.bindingInfo.type             = DescriptorType::kUniformBuffer;
                        descriptor.bindingInfo.stage            = ShaderStage(ShaderStage::kPixel);
                        descriptor.uniformBuffer                = lightsDataBuffer;

                        RZDescriptorSetDesc setCreateDesc = {};
                        setCreateDesc.name                = "DescriptorSet.SceneLightsData";
                        setCreateDesc.heapType            = DescriptorHeapType::kCbvUavSrvHeap;
                        setCreateDesc.setIdx              = BindingTable_System::SET_IDX_FRAME_DATA;
                        setCreateDesc.descriptors.push_back(descriptor);

                        auto m_SceneLightsDataSet = RZResourceManager::Get().createDescriptorSet(setCreateDesc);
                        Gfx::RHI::Get().setSceneLightsDataSet(m_SceneLightsDataSet);
                    }
                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }
#endif
    }    // namespace Gfx
}    // namespace Razix
