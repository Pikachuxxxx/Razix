// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZWorldRenderer.h"

#define ENABLE_CODE_DRIVEN_FG_PASSES 0

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RZGraphicsCompileConfig.h"

#include "Razix/Gfx/FrameGraph/RZBlackboard.h"
#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"

#include "Razix/Gfx/Lighting/RZImageBasedLightingProbesManager.h"

#include "Razix/Gfx/Passes/Data/BRDFData.h"
#include "Razix/Gfx/Passes/Data/FrameData.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
#include "Razix/Gfx/RHI/API/RZShader.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"

#include "Razix/Gfx/Renderers/RZDebugRendererProxy.h"
#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Math/ImportanceSampling.h"
#include "Razix/Math/RZGrid.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Utilities/RZColorUtilities.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/plugins/IconsFontAwesome5.h>

// TODO: Test per frame write for RT and read only for shader like resource and use caspture by reference for execute lambda in fg functions

namespace Razix {
    namespace Gfx {

        static void ExportFrameGraphVisFile(const FrameGraph::RZFrameGraph& framegraph)
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

        //-------------------------------------------------------------------------------------------

        /**
         * Notes:
         * 1. In Razix we use CW winding order for front facing triangles => Also, back facing faces are pointed towards the camera
         */

        void RZWorldRenderer::buildFrameGraph(RZRendererSettings& settings, Razix::RZScene* scene)
        {
            m_FrameGraphBuildingInProgress = true;

            // Noise texture LUT
            RZTextureDesc noiseDesc     = {};
            noiseDesc.name              = "VolumetricCloudsNoise";    // must match shader
            noiseDesc.enableMips        = false;
            noiseDesc.filePath          = "//RazixContent/Textures/Texture.Builtin.VolumetricCloudsNoise.png";
            m_NoiseTextureHandle        = RZResourceManager::Get().createTexture(noiseDesc);
            auto& volumetricData        = m_FrameGraph.getBlackboard().add<VolumetricCloudsData>();
            volumetricData.noiseTexture = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>(noiseDesc.name, CAST_TO_FG_TEX_DESC noiseDesc, {m_NoiseTextureHandle});

            // Import the color grading LUT
            RZTextureDesc colorGradingNeutralLUTDesc = {};
            colorGradingNeutralLUTDesc.name          = "ColorGradingUnreal_Neutral_LUT16";    // must match shader
            colorGradingNeutralLUTDesc.enableMips    = false;
            colorGradingNeutralLUTDesc.flipY         = true;
            colorGradingNeutralLUTDesc.filePath      = "//RazixContent/Textures/Texture.Builtin.ColorGradingNeutralLUT16.png";
            m_ColorGradingNeutralLUTHandle           = RZResourceManager::Get().createTexture(colorGradingNeutralLUTDesc);
            auto& colorGradingLUTSData               = m_FrameGraph.getBlackboard().add<FX::ColorGradingLUTData>();
            colorGradingLUTSData.neutralLUT          = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>(colorGradingNeutralLUTDesc.name, CAST_TO_FG_TEX_DESC colorGradingNeutralLUTDesc, {m_ColorGradingNeutralLUTHandle});

            // Upload buffers/textures Data to the FrameGraph and GPU initially
            // Upload BRDF look up texture to the GPU
            RZTextureDesc brdfDesc  = {};
            brdfDesc.name           = "BrdfLUT";    // must match shader
            brdfDesc.enableMips     = false;
            brdfDesc.filePath       = "//RazixContent/Textures/Texture.Builtin.BrdfLUT.png";
            m_BRDFfLUTTextureHandle = RZResourceManager::Get().createTexture(brdfDesc);
            auto& brdfData          = m_FrameGraph.getBlackboard().add<BRDFData>();
            brdfData.lut            = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>(brdfDesc.name, CAST_TO_FG_TEX_DESC brdfDesc, {m_BRDFfLUTTextureHandle});

            //-----------------------------------------------------------------------------------

            // Load the Skybox and Global Light Probes
            // FIXME: This is hard coded make this a user land material
            m_GlobalLightProbes.skybox   = RZImageBasedLightingProbesManager::convertEquirectangularToCubemap("//RazixContent/Textures/HDR/sunset.hdr");
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
            GBufferData& gBufferData = m_FrameGraph.getBlackboard().get<GBufferData>();

            //-------------------------------
            // PBR Deferred Pass
            //-------------------------------
            m_PBRDeferredPass.addPass(m_FrameGraph, scene, &settings);
            auto& sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            //-------------------------------
            // Debug Scene Pass
            //-------------------------------
            m_FrameGraph.addCallbackPass(
                "Pass.Builtin.Code.DebugDraw",
                [&](auto& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZDebugRendererProxy::Get().Init();

                    builder.read(frameDataBlock.frameData);
                    builder.read(sceneData.sceneHDR);
                    builder.read(sceneData.sceneDepth);
                },
                [=](const auto& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("DebugDraw Pass");

                    // Origin point
                    //RZDebugRendererProxy::DrawPoint(glm::vec3(0.0f), 0.25f);

                    // X, Y, Z lines
                    RZDebugRendererProxy::DrawLine(glm::vec3(-100.0f, 0.0f, 0.0f), glm::vec3(100.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
                    RZDebugRendererProxy::DrawLine(glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f, 100.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                    RZDebugRendererProxy::DrawLine(glm::vec3(0.0f, 0.0f, -100.0f), glm::vec3(0.0f, 0.0f, 100.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

                    // Grid
                    RZDebugRendererProxy::DrawGrid(125, glm::vec4(0.75f));

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

                    glm::mat4 lightView  = glm::lookAt(dir_light.getPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    float     near_plane = -50.0f, far_plane = 50.0f;
                    glm::mat4 lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
                    lightProjection[1][1] *= -1;
                    auto lightViewProj = lightProjection * lightView;
                    RZDebugRendererProxy::DrawFrustum(lightViewProj, glm::vec4(0.863f, 0.28f, 0.21f, 1.0f));

                    // Draw all camera frustums
                    auto cameras = scene->GetComponentsOfType<CameraComponent>();
                    for (auto& camComponents: cameras) {
                        RZDebugRendererProxy::DrawFrustum(camComponents.Camera.getFrustum(), glm::vec4(0.2f, 0.85f, 0.1f, 1.0f));
                    }

                    // Draw AABBs for all the Meshes in the Scene
                    auto mesh_group = scene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
                    for (auto entity: mesh_group) {
                        // Draw the mesh renderer components
                        const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                        // Bind push constants, VBO, IBO and draw
                        glm::mat4 transform = mesh_trans.GetGlobalTransform();

                        if (mrc.Mesh && mrc.enableBoundingBoxes)
                            RZDebugRendererProxy::DrawAABB(mrc.Mesh->getBoundingBox().transform(transform), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                    }

                    RZDebugRendererProxy::Get().Begin(scene);

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneHDR).getHandle();
                    auto dt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneDepth).getHandle();

                    RenderingInfo info    = {};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{rt, {false, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {dt, {false, ClearColorPresets::DepthOneToZero}};
                    info.resize           = true;

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
            m_FrameGraph.addCallbackPass(
                "Pass.Builtin.Code.ImGui",
                [&](auto&, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    builder.read(sceneData.sceneHDR);
                    builder.read(sceneData.sceneDepth);

                    RZImGuiRendererProxy::Get().Init();
                },
                [=](const auto&, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("ImGui Pass");

                    RZImGuiRendererProxy::Get().Begin(scene);

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneHDR).getHandle();
                    auto dt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneDepth).getHandle();

                    RenderingInfo info    = {};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{rt, {false, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {dt, {false, ClearColorPresets::DepthOneToZero}};
                    info.resize           = true;

                    RHI::BeginRendering(Gfx::RHI::GetCurrentCommandBuffer(), info);

                    if (settings.renderFeatures & RendererFeature_ImGui)
                        RZImGuiRendererProxy::Get().Draw(Gfx::RHI::GetCurrentCommandBuffer());

                    RZImGuiRendererProxy::Get().End();
                    RAZIX_TIME_STAMP_END();
                });

            sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            //-------------------------------
            // Skybox Pass
            //-------------------------------
            m_SkyboxPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // Composition Pass
            //-------------------------------
            m_FrameGraph.getBlackboard().setFinalOutputName("SceneHDR");
            m_CompositePass.addPass(m_FrameGraph, scene, &settings);

            // Compile the Frame Graph
            RAZIX_CORE_INFO("Compiling FrameGraph....");
            m_FrameGraph.compile();

            // Dump the Frame Graph for visualization
            // NOTE: Careful this won't write to the Engine directory this is inside bin and build artifact
            // FIXME: Find a way to map VFS to OG Engine path pre-copy or idk just umm...be careful I guess
            ExportFrameGraphVisFile(m_FrameGraph);

            m_FrameGraphBuildingInProgress = false;
        }

        void RZWorldRenderer::drawFrame(RZRendererSettings& settings, Razix::RZScene* scene)
        {
            m_FrameCount++;

            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (m_IsFGFilePathDirty) {
                destroy();
                FrameGraph::RZFrameGraph::ResetFirstFrame();
                buildFrameGraph(settings, RZSceneManager::Get().getCurrentScene());
                m_IsFGFilePathDirty = false;
            }

            if (m_FrameGraphBuildingInProgress)
                return;

            // Main Frame Graph World Rendering Loop
            {
                // Acquire Image to render onto
                Gfx::RHI::AcquireImage(nullptr);

                // Begin Recording  onto the command buffer, select one as per the frame idx
                Gfx::RHI::Begin(Gfx::RHI::GetCurrentCommandBuffer());

                // Begin Frame Marker
                RAZIX_MARK_BEGIN("Frame # " + std::to_string(m_FrameCount) + " [back buffer # " + std::to_string(RHI::GetSwapchain()->getCurrentFrameIndex()) + " ]", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

                // Execute the Frame Graph passes
                m_FrameGraph.execute(nullptr);

                // End Frame Marker
                RAZIX_MARK_END();

                // Submit the render queue before presenting next
                Gfx::RHI::Submit(Gfx::RHI::GetCurrentCommandBuffer());

                // swapchain capture is done before presentation
                if (m_ReadSwapchainThisFrame) {
                    m_ReadSwapchainThisFrame = false;

                    // Wait for rendering to be done before capturing
                    RZGraphicsContext::GetContext()->Wait();

                    // Use a single time command buffer to this
                    RZDrawCommandBufferHandle cmdBuff = RZDrawCommandBuffer::BeginSingleTimeCommandBuffer("Swapchain Capture", Utilities::GenerateHashedColor4(24u));

                    m_LastSwapchainReadback = RHI::InsertTextureReadback(cmdBuff, RHI::GetSwapchain()->GetCurrentBackBufferImage());

                    RZDrawCommandBuffer::EndSingleTimeCommandBuffer(cmdBuff);
                }

                // Present the image to presentation engine as soon as rendering to SCOLOR_ATTACHMENT is done
                Gfx::RHI::Present(nullptr);
            }
        }

        void RZWorldRenderer::destroy()
        {
            m_FrameCount = 0;

            if (m_LastSwapchainReadback.data) {
                free(m_LastSwapchainReadback.data);
                m_LastSwapchainReadback.data = NULL;
            }

            // Wait for rendering to be done before halting
            Gfx::RZGraphicsContext::GetContext()->Wait();

            m_FrameGraphBuildingInProgress = true;

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
            // m_VisBufferFillPass.destroy();
            m_CompositePass.destroy();

            // Wait for GPU to be done
            Gfx::RZGraphicsContext::GetContext()->Wait();
        }

        void RZWorldRenderer::OnUpdate(RZTimestep dt)
        {
        }

        void RZWorldRenderer::OnImGui()
        {
            if (ImGui::Begin("FrameGraph Debug")) {
                if (ImGui::CollapsingHeader("Debug Texture Pool View (All)")) {
                    auto& texturePool = RZResourceManager::Get().getPool<RZTexture>();

                    for (auto& textureHandle: texturePool.getHandles()) {
                        auto textureResource = RZResourceManager::Get().getTextureResource(textureHandle);
                        ImGui::Text("%s", textureResource->getDescription().name.c_str());
                    }
                }
            }
            ImGui::End();

            //-------------------------------------------------------------------
            if (RZApplication::Get().getAppType() == AppType::GAME) {
                // Engine ImGui Tools will be rendered here
                static bool showResourceViewer = false;
                static bool showBudgets        = false;
                static bool showMemStats       = true;
                static bool showRHIStats       = true;

                // RAZIX_PROFILE_SCOPEC("Engine Tools", RZ_PROFILE_COLOR_CORE)
                RAZIX_PROFILE_SCOPEC("Engine Tools", RZ_PROFILE_COLOR_CORE)

                if (ImGui::BeginMainMenuBar()) {
                    if (ImGui::BeginMenu(ICON_FA_WRENCH " Tools")) {
                        if (ImGui::MenuItem(ICON_FA_TASKS " FG resource Viewer", nullptr, showResourceViewer)) {
                            showResourceViewer = !showResourceViewer;
                        }
                        if (ImGui::MenuItem(ICON_FA_MONEY_BILL " Frame Budgets", nullptr, showBudgets)) {
                            showBudgets = !showBudgets;
                        }
                        if (ImGui::MenuItem(ICON_FA_MEMORY " Memory Stats", nullptr, showMemStats)) {
                            showMemStats = !showMemStats;
                        }
                        if (ImGui::MenuItem(ICON_FA_MEMORY " RHI Memory Stats", nullptr, showRHIStats)) {
                            showRHIStats = !showRHIStats;
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMainMenuBar();
                }
                // RAZIX_PROFILE_SCOPEC("Engine Tools", RZ_PROFILE_COLOR_CORE)

                //==========================================================================

                // Memory Stats
                if (showMemStats) {
                    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
                    ImGui::SetNextWindowBgAlpha(1.0f);    // Transparent background
                    ImGui::SetNextWindowSize(ImVec2((f32) RZApplication::Get().getWindow()->getWidth(), 150.0f));
                    ImGui::SetNextWindowPos(ImVec2(0.0f, (f32) RZApplication::Get().getWindow()->getHeight() - 50), ImGuiCond_Always);
                    ImGui::Begin("##MemStats", 0, window_flags);
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
                        ImGui::Text(ICON_FA_MEMORY "  GPU Memory: %4.2f", RZEngine::Get().GetStatistics().TotalGPUMemory);
                        ImGui::PopStyleColor(1);
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
                        ImGui::Text(ICON_FA_BALANCE_SCALE " Used GPU Memory: %4.2f |", RZEngine::Get().GetStatistics().GPUMemoryUsed);
                        ImGui::PopStyleColor(1);

                        ImGui::SameLine();
                        std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                        ImGui::Text(ICON_FA_CLOCK " current date/time : %s ", std::ctime(&end_time));
                        ImGui::PopStyleColor(1);

                        ImGui::SameLine();

                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                        std::string engineBuildVersionFull = RazixVersion.getVersionString() + "." + RazixVersion.getReleaseStageString();
                        ImGui::Text("| Engine build version : %s | ", engineBuildVersionFull.c_str());
                        ImGui::PopStyleColor(1);

                        ImGui::SameLine();

                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                        ImGui::Text(ICON_FA_ID_CARD " project UUID : %s", RZApplication::Get().getProjectUUID().prettyString().c_str());
                        ImGui::PopStyleColor(1);
                    }
                    ImGui::End();
                }
                // Memory Stats
            }
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

            globalLightProbeData.environmentMap         = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("EnvironmentMap", CAST_TO_FG_TEX_DESC SkyboxDesc, {globalLightProbe.skybox});
            globalLightProbeData.diffuseIrradianceMap   = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("IrradianceMap", CAST_TO_FG_TEX_DESC DiffuseDesc, {globalLightProbe.diffuse});
            globalLightProbeData.specularPreFilteredMap = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("PreFilteredMap", CAST_TO_FG_TEX_DESC SpecularDesc, {globalLightProbe.specular});
        }

        //--------------------------------------------------------------------------

        void RZWorldRenderer::uploadFrameData(RZScene* scene, RZRendererSettings& settings)
        {
            m_FrameGraph.getBlackboard().add<FrameData>() = m_FrameGraph.addCallbackPass<FrameData>(
                "Pass.Builtin.Code.FrameDataUpload",
                [&](FrameData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    // TODO: convert this to struct form
                    data.frameData = builder.create<FrameGraph::RZFrameGraphBuffer>("FrameData", {"FrameData", sizeof(GPUFrameData), 0, BufferUsage::PersistentStream});

                    data.frameData = builder.write(data.frameData);
                },
                [=](const FrameData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("Upload FrameData");
                    RAZIX_MARK_BEGIN("Upload FrameData", glm::vec4(0.8f, 0.2f, 0.15f, 1.0f));

                    GPUFrameData gpuData{};
                    gpuData.time += gpuData.deltaTime;
                    gpuData.deltaTime      = RZEngine::Get().GetStatistics().DeltaTime;
                    gpuData.resolution     = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    gpuData.debugFlags     = settings.debugFlags;
                    gpuData.renderFeatures = settings.renderFeatures;

                    m_Jitter = m_TAAJitterHaltonSamples[(m_FrameCount % NUM_HALTON_SAMPLES_TAA_JITTER)];
                    // Based on scene sampling pattern set the apt jitter
                    if (RZEngine::Get().getWorldSettings().samplingPattern == Halton)
                        gpuData.jitterTAA = m_Jitter;

                    gpuData.previousJitterTAA = m_PreviousJitter;

                    auto& sceneCam = scene->getSceneCamera();

                    sceneCam.setAspectRatio(f32(RZApplication::Get().getWindow()->getWidth()) / f32(RZApplication::Get().getWindow()->getHeight()));

                    // clang-format off
                    glm::mat4 jitterMatrix = glm::mat4(
                        1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        gpuData.jitterTAA.x, gpuData.jitterTAA.y, 0.0, 1.0    // translation
                    );
                    // clang-format on

                    //auto jitteredProjMatrix = sceneCam.getProjection() * jitterMatrix;

                    gpuData.camera.projection         = sceneCam.getProjection();
                    gpuData.camera.inversedProjection = glm::inverse(gpuData.camera.projection);
                    gpuData.camera.view               = sceneCam.getViewMatrix();
                    gpuData.camera.inversedView       = glm::inverse(gpuData.camera.view);
                    gpuData.camera.prevViewProj       = m_PreviousViewProj;
                    gpuData.camera.fov                = sceneCam.getPerspectiveVerticalFOV();
                    gpuData.camera.nearPlane          = sceneCam.getPerspectiveNearClip();
                    gpuData.camera.farPlane           = sceneCam.getPerspectiveFarClip();

                    // update and upload the UBO
                    auto frameDataBufferHandle = resources.get<FrameGraph::RZFrameGraphBuffer>(data.frameData).getHandle();
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
                [&](SceneLightsData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    // TODO: convert this to struct form
                    data.lightsDataBuffer = builder.create<FrameGraph::RZFrameGraphBuffer>("SceneLightsData", {"SceneLightsData", sizeof(GPULightsData), 0, BufferUsage::PersistentStream});
                    data.lightsDataBuffer = builder.write(data.lightsDataBuffer);
                },
                [=](const SceneLightsData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("Upload SceneLights");
                    RAZIX_MARK_BEGIN("Upload SceneLights", glm::vec4(0.2f, 0.2f, 0.75f, 1.0f));

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
                    auto lightsDataBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(data.lightsDataBuffer).getHandle();
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
    }    // namespace Gfx
}    // namespace Razix