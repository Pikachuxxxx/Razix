// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZWorldRenderer.h"

#define ENABLE_CODE_DRIVEN_FG_PASSES 0
#define ENABLE_FORWARD_RENDERING     0

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/FrameGraph/RZBlackboard.h"
#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"

#include "Razix/Gfx/Lighting/RZImageBasedLightingProbesManager.h"

#include "Razix/Gfx/Passes/Data/BRDFData.h"
#include "Razix/Gfx/Passes/Data/FrameData.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"

#include "Razix/Gfx/Renderers/RZDebugRenderer.h"
#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Maths/HaltonSeq.h"
#include "Razix/Maths/RZGrid.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Gfx/RZGraphicsCompileConfig.h"

#include <imgui/imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace Razix {

    namespace Gfx {

        /**
         * Notes:
         * 1. In Razix we use CW winding order for front facing triangles => Also, back facing faces are pointed towards the camera
         */

        void RZWorldRenderer::buildFrameGraph(RZRendererSettings& settings, Razix::RZScene* scene)
        {
#if ENABLE_TEST_PASSES

            //-------------------------------
            // [TEST] HELLO TRIANGLE
            //-------------------------------
            m_HelloTriangleTestPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // [TEST] WAVE INTRINSICE
            //-------------------------------
            m_WaveInstrinsicsTestPass.addPass(m_FrameGraph, scene, &settings);

            // These are system level code passes so always enabled
            uploadFrameData(scene, settings);
            //            auto& frameDataBlock = m_FrameGraph.getBlackboard().get<FrameData>();

            m_GlobalLightProbes.skybox   = RZImageBasedLightingProbesManager::convertEquirectangularToCubemap("//Assets/Textures/HDR/newport_loft.hdr");
            m_GlobalLightProbes.diffuse  = RZImageBasedLightingProbesManager::generateIrradianceMap(m_GlobalLightProbes.skybox);
            m_GlobalLightProbes.specular = RZImageBasedLightingProbesManager::generatePreFilteredMap(m_GlobalLightProbes.skybox);

            //-------------------------------
            // [TEST] GS CUBE
            //-------------------------------
            // m_GSCubeTestPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // [TEST] HELLO TEXTURE
            //-------------------------------
            m_HelloTextureTestPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // Vis Buffer Fill Pass
            //-------------------------------
            //m_VisBufferFillPass.addPass(m_FrameGraph, scene, &settings);
            return;
#else

            m_FrameGraphBuildingInProgress = true;

            // Upload buffers/textures Data to the FrameGraph and GPU initially
            // Upload BRDF look up texture to the GPU
            RZTextureDesc brdfDesc{};
            brdfDesc.name                                    = "BrdfLUT";
            brdfDesc.enableMips                              = false;
            brdfDesc.filePath                                = "//RazixContent/Textures/Texture.Builtin.BrdfLUT.png";
            brdfDesc.filtering                               = {Filtering::Mode::kFilterModeLinear, Filtering::Mode::kFilterModeLinear};
            brdfDesc.wrapping                                = Wrapping::kRepeat;
            m_BRDFfLUTTextureHandle                          = RZResourceManager::Get().createTexture(brdfDesc);
            m_FrameGraph.getBlackboard().add<BRDFData>().lut = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>(brdfDesc.name, CAST_TO_FG_TEX_DESC brdfDesc, {m_BRDFfLUTTextureHandle});

            // Noise texture LUT
            RZTextureDesc noiseDesc{};
            noiseDesc.name                                                        = "VolumetricCloudsNoise";
            noiseDesc.enableMips                                                  = false;
            noiseDesc.filePath                                                    = "//RazixContent/Textures/Texture.Builtin.VolumetricCloudsNoise.png";
            noiseDesc.wrapping                                                    = Wrapping::kRepeat;
            m_NoiseTextureHandle                                                  = RZResourceManager::Get().createTexture(noiseDesc);
            m_FrameGraph.getBlackboard().add<VolumetricCloudsData>().noiseTexture = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>(noiseDesc.name, CAST_TO_FG_TEX_DESC noiseDesc, {m_NoiseTextureHandle});

            // Import the color grading LUT
            RZTextureDesc colorGradingNeutralLUTDesc{};
            colorGradingNeutralLUTDesc.name                                        = "ColorGradingUnreal_Neutral_LUT16";
            colorGradingNeutralLUTDesc.enableMips                                  = false;
            colorGradingNeutralLUTDesc.flipY                                       = true;
            colorGradingNeutralLUTDesc.filePath                                    = "//RazixContent/Textures/Texture.Builtin.ColorGradingNeutralLUT16.png";
            colorGradingNeutralLUTDesc.filtering                                   = {Filtering::Mode::kFilterModeLinear, Filtering::Mode::kFilterModeLinear};
            colorGradingNeutralLUTDesc.wrapping                                    = Wrapping::kRepeat;
            m_ColorGradingNeutralLUTHandle                                         = RZResourceManager::Get().createTexture(colorGradingNeutralLUTDesc);
            m_FrameGraph.getBlackboard().add<FX::ColorGradingLUTData>().neutralLUT = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>(colorGradingNeutralLUTDesc.name, CAST_TO_FG_TEX_DESC colorGradingNeutralLUTDesc, {m_ColorGradingNeutralLUTHandle});

            //-----------------------------------------------------------------------------------

            // Load the Skybox and Global Light Probes
            // FIXME: This is hard coded make this a user land material
            m_GlobalLightProbes.skybox = RZImageBasedLightingProbesManager::convertEquirectangularToCubemap("//RazixContent/Textures/HDR/sunset.hdr");
            //m_GlobalLightProbes.diffuse  = RZImageBasedLightingProbesManager::generateIrradianceMap(m_GlobalLightProbes.skybox);
            //m_GlobalLightProbes.specular = RZImageBasedLightingProbesManager::generatePreFilteredMap(m_GlobalLightProbes.skybox);
            // Import this into the Frame Graph
            //importGlobalLightProbes(m_GlobalLightProbes);

            //-----------------------------------------------------------------------------------
            // Misc Variables

            for (int i = 0; i < NUM_HALTON_SAMPLES_TAA_JITTER; ++i) {
                // Generate jitter using Halton sequence with bases 2 and 3 for X and Y respectively
                m_TAAJitterHaltonSamples[i].x = 2.0f * (f32) (HaltonSequence(i + 1, 2) - 1.0f);    // Centering the jitter around (0,0)
                m_TAAJitterHaltonSamples[i].y = 2.0f * (f32) (HaltonSequence(i + 1, 3) - 1.0f);
                m_TAAJitterHaltonSamples[i].x /= RZApplication::Get().getWindow()->getWidth();
                m_TAAJitterHaltonSamples[i].y /= RZApplication::Get().getWindow()->getHeight();
            }

            //-----------------------------------------------------------------------------------

            // Cull Lights (Directional + Point) on CPU against camera Frustum First
            // TODO: Get the list of lights in the scene and cull them against the camera frustum and disable ActiveComponent for culled lights, but for now we can just ignore that
            auto                 group = scene->getRegistry().group<LightComponent>(entt::get<TransformComponent>);
            std::vector<RZLight> sceneLights;
            for (auto& entity: group)
                sceneLights.push_back(group.get<LightComponent>(entity).light);

            // Pass the Scene AABB and Grid info for GI + Tiled lighting
            // TODO: Make this dynamic as scene glows larger
            m_SceneAABB = {glm::vec3(-76.83, -5.05, -47.31), glm::vec3(71.99, 57.17, 44.21)};
            const Maths::RZGrid sceneGrid(m_SceneAABB);

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
            // Testing disabled shadows
            //settings.renderFeatures &= ~RendererFeature_Shadows;

            //-------------------------------
            // Simple Shadow map Pass
            //-------------------------------
            m_ShadowPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // [ ] CSM PAss
            //-------------------------------
            m_CSMPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // GBuffer Pass
            //-------------------------------
            m_GBufferPass.addPass(m_FrameGraph, scene, &settings);
            GBufferData& gBufferData = m_FrameGraph.getBlackboard().get<GBufferData>();

    #if !ENABLE_FORWARD_RENDERING
            //-------------------------------
            // SSAO Pass
            //-------------------------------
            //settings.renderFeatures |= RendererFeature_SSAO;
            settings.renderFeatures &= ~RendererFeature_SSAO;
            //if (settings.renderFeatures & RendererFeature_SSAO)
            //    settings.renderFeatures |= RendererFeature_SSAO;
            //m_SSAOPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // Gaussian Blur Pass - SSAO
            //-------------------------------
            //auto& ssaoData = m_FrameGraph.getBlackboard().get<FX::SSAOData>();
            //m_GaussianBlurPass.setTwoPassFilter(false);
            //m_GaussianBlurPass.setBlurRadius(1.0f);
            //m_GaussianBlurPass.setFilterTap(GaussianTap::Five);
            //m_GaussianBlurPass.setInputTexture(ssaoData.SSAOPreBlurTexture);
            //m_GaussianBlurPass.addPass(m_FrameGraph, scene, &settings);
            //ssaoData.SSAOSceneTexture = m_GaussianBlurPass.getOutputTexture();

            //-------------------------------
            // PBR Deferred Pass
            //-------------------------------
            m_PBRDeferredPass.addPass(m_FrameGraph, scene, &settings);
    #endif

            //-------------------------------
            // PBR Forward Pass
            //-------------------------------
    #if ENABLE_FORWARD_RENDERING
            m_PBRLightingPass.addPass(m_FrameGraph, scene, settings);
    #endif
            SceneData& sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            //-------------------------------
            // Skybox Pass
            //-------------------------------
            m_SkyboxPass.addPass(m_FrameGraph, scene, &settings);
            sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            //-------------------------------
            // [] Color Grading LUT Pass
            //-------------------------------
            //m_ColorGradingPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // [] TAA Resolve Pass
            //-------------------------------
            //m_TAAResolvePass.addPass(m_FrameGraph, scene, &settings);
            //sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            //-------------------------------
            // Tonemapping Pass
            //-------------------------------
            m_TonemapPass.addPass(m_FrameGraph, scene, &settings);
            sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            //-------------------------------
            // FXAA Pass
            //-------------------------------
            m_FXAAPass.addPass(m_FrameGraph, scene, &settings);
            sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            /**
             * These code driven passes only work with the Render Targets names SceneHDR and SceneDepth, 
             * as of now they are necessary to be found and we will write to them!!!
             * 
             * Basically for data driven passes you need data!
             * 
             * Potential BUG: cloning of resources in StringBased blackboard won't track older read/write IDs FIX IT! 
             * BUG: CLONING DOESN'T WORK IN DATA DRIVEN RENDERING!!!
             * PLAUSIBLE DESING FIX: Use the version prefix when adding to blackboard and use that while retrieving this way we can have uniqueness in the unordered map
             * Workaround: have unique resources names
             */

            //-------------------------------
            // Debug Scene Pass
            //-------------------------------
            m_FrameGraph.addCallbackPass(
                "Pass.Builtin.Code.Debug",
                [&](auto& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZDebugRenderer::Get()->Init();

                    // TODO: Make these read/write safe, use hasID from blackboard and only then enable these passes
                    // or register the Scene RTs similar to FinalOutputTarget to get this working safely

                    builder.read(frameDataBlock.frameData);
                    builder.read(sceneData.sceneHDR);
                    builder.read(sceneData.sceneDepth);

                    sceneData.sceneHDR   = builder.write(sceneData.sceneHDR);
                    sceneData.sceneDepth = builder.write(sceneData.sceneDepth);
                },
                [=](const auto& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("DebugDraw Pass");

                    // Origin point
                    RZDebugRenderer::DrawPoint(glm::vec3(0.0f), 0.1f);

                    // X, Y, Z lines
                    RZDebugRenderer::DrawLine(glm::vec3(-100.0f, 0.0f, 0.0f), glm::vec3(100.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
                    RZDebugRenderer::DrawLine(glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f, 100.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                    RZDebugRenderer::DrawLine(glm::vec3(0.0f, 0.0f, -100.0f), glm::vec3(0.0f, 0.0f, 100.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

                    // Grid
                    RZDebugRenderer::DrawGrid(25, glm::vec4(0.75f));

                    // Draw all lights in the scene
                    auto lights = scene->GetComponentsOfType<LightComponent>();
                    for (auto& lightComponent: lights) {
                        RZDebugRenderer::DrawLight(&lights[0].light, glm::vec4(0.8f, 0.65f, 0.0f, 1.0f));
                    }

                    // Draw CSM frustums
                    auto cascades = m_CSMPass.getCascades();
                    for (u32 i = 0; i < cascades.size(); i++) {
                        RZDebugRenderer::DrawFrustum(cascades[i].viewProjMatrix, glm::vec4(0.72f, 0.85f, 0.1f * i, 1.0f));
                    }

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
                    RZDebugRenderer::DrawFrustum(lightViewProj, glm::vec4(0.863f, 0.28f, 0.21f, 1.0f));

                    // Draw all camera frustums
                    auto cameras = scene->GetComponentsOfType<CameraComponent>();
                    for (auto& camComponents: cameras) {
                        RZDebugRenderer::DrawFrustum(camComponents.Camera.getFrustum(), glm::vec4(0.2f, 0.85f, 0.1f, 1.0f));
                    }

                    // Draw AABBs for all the Meshes in the Scene
                    auto mesh_group = scene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
                    for (auto entity: mesh_group) {
                        // Draw the mesh renderer components
                        const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                        // Bind push constants, VBO, IBO and draw
                        glm::mat4 transform = mesh_trans.GetGlobalTransform();

                        if (mrc.Mesh && mrc.enableBoundingBoxes)
                            RZDebugRenderer::DrawAABB(mrc.Mesh->getBoundingBox().transform(transform), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                    }

                    RZDebugRenderer::Get()->Begin(scene);

                    //auto sceneHDR   = m_FrameGraph.getBlackboard().getID("SceneHDR");
                    //auto sceneDepth = m_FrameGraph.getBlackboard().getID("SceneDepth");

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneHDR).getHandle();
                    auto dt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneDepth).getHandle();

                    RenderingInfo info{};
                    info.resolution       = Resolution::kCustom;
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.colorAttachments = {{rt, {false, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {dt, {false, ClearColorPresets::DepthOneToZero}};
                    info.resize           = true;

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RHI::BeginRendering(cmdBuffer, info);

                    RZDebugRenderer::Get()->Draw(cmdBuffer);

                    RHI::EndRendering(cmdBuffer);

                    RZDebugRenderer::Get()->End();
                    RAZIX_TIME_STAMP_END();
                });

            sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            //-------------------------------
            // ImGui Pass
            //-------------------------------
            m_FrameGraph.addCallbackPass(
                "Pass.Builtin.Code.ImGui",
                [&](auto&, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    builder.read(sceneData.sceneHDR);
                    builder.read(sceneData.sceneDepth);

                    sceneData.sceneHDR   = builder.write(sceneData.sceneHDR);
                    sceneData.sceneDepth = builder.write(sceneData.sceneDepth);

                    m_ImGuiRenderer.Init();
                },
                [&](const auto&, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("ImGui Pass");

                    m_ImGuiRenderer.Begin(scene);

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneHDR).getHandle();
                    auto dt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneDepth).getHandle();

                    //auto sceneHDR   = m_FrameGraph.getBlackboard().getID("SceneHDR");
                    //auto sceneDepth = m_FrameGraph.getBlackboard().getID("SceneDepth");

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{rt, {false, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {dt, {false, ClearColorPresets::DepthOneToZero}};
                    info.resize           = true;

                    RHI::BeginRendering(Gfx::RHI::GetCurrentCommandBuffer(), info);

                    if (settings.renderFeatures & RendererFeature_ImGui)
                        m_ImGuiRenderer.Draw(Gfx::RHI::GetCurrentCommandBuffer());

                    m_ImGuiRenderer.End();
                    RAZIX_TIME_STAMP_END();
                });

            sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            //-------------------------------
            // Composition Pass
            //-------------------------------
            m_CompositePass.addPass(m_FrameGraph, scene, &settings);

#endif

            // Compile the Frame Graph
            RAZIX_CORE_INFO("Compiling FrameGraph ....");
            m_FrameGraph.compile();

            // Dump the Frame Graph for visualization
            // NOTE: Careful this won't write to the Engine directory this is inside bin and build artifact
            // FIXME: Find a way to map VFS to OG Engine path pre-copy or idk just umm...be careful I guess
            std::string outPath;
            RZVirtualFileSystem::Get().resolvePhysicalPath("//RazixContent/FrameGraphs", outPath, true);
            RAZIX_CORE_INFO("Exporting FrameGraph .... to ({0})", outPath);
            std::ofstream os(outPath + "/data_driven_editor_test.dot");
            os << m_FrameGraph;

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

            //if (m_FrameGraphBuildingInProgress)
            //    return;

            // Update calls passes
            //m_CSMPass.updateCascades(scene);

            // Main Frame Graph World Rendering Loop
            {
                // Acquire Image to render onto

                Gfx::RHI::AcquireImage(nullptr);

                // Begin Recording  onto the command buffer, select one as per the frame idx
                RHI::Begin(Gfx::RHI::GetCurrentCommandBuffer());

                // Begin Frame Marker
                RAZIX_MARK_BEGIN("Frame # " + std::to_string(m_FrameCount) + " [back buffer # " + std::to_string(RHI::GetSwapchain()->getCurrentFrameIndex()) + " ]", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

                // Execute the Frame Graph passes
                m_FrameGraph.execute(nullptr);

                // End Frame Marker
                RAZIX_MARK_END();

                // Submit the render queue before presenting next
                Gfx::RHI::Submit(Gfx::RHI::GetCurrentCommandBuffer());

                // Present the image to presentation engine as soon as rendering to SCOLOR_ATTACHMENT is done
                Gfx::RHI::Present(nullptr);
            }
        }

        void RZWorldRenderer::destroy()
        {
            m_FrameCount = 0;

            // Wait for rendering to be done before halting
            Gfx::RZGraphicsContext::GetContext()->Wait();

            m_FrameGraphBuildingInProgress = true;

#if ENABLE_TEST_PASSES
            m_HelloTriangleTestPass.destroy();
            m_HelloTextureTestPass.destroy();
            m_WaveInstrinsicsTestPass.destroy();

            //m_VisBufferFillPass.destroy();
            RZResourceManager::Get().destroyTexture(m_GlobalLightProbes.skybox);
            RZResourceManager::Get().destroyTexture(m_GlobalLightProbes.diffuse);
            RZResourceManager::Get().destroyTexture(m_GlobalLightProbes.specular);
#else

            // Destroy Imported Resources
            RZResourceManager::Get().destroyTexture(m_NoiseTextureHandle);
            RZResourceManager::Get().destroyTexture(m_BRDFfLUTTextureHandle);
            RZResourceManager::Get().destroyTexture(m_ColorGradingNeutralLUTHandle);

            // Destroy Renderers
            m_ImGuiRenderer.Destroy();
            RZDebugRenderer::Get()->Destroy();

            // Destroy Passes
    #if !ENABLE_FORWARD_RENDERING
            m_PBRDeferredPass.destroy();
    #else
            m_PBRLightingPass.destroy();
    #endif
            m_SkyboxPass.destroy();
            m_ShadowPass.destroy();
            m_GBufferPass.destroy();
            m_SSAOPass.destroy();
            m_GaussianBlurPass.destroy();
            m_CompositePass.destroy();

#endif

            // Destroy Frame Graph Transient Resources
            m_FrameGraph.destroy();

            // Wait for GPU to be done
            Gfx::RZGraphicsContext::GetContext()->Wait();
        }

        void RZWorldRenderer::OnUpdate(RZTimestep dt)
        {
        }

        void RZWorldRenderer::OnImGui()
        {
            //auto texHandle   = m_CSMPass.getCSMArrayTex();
            //auto texResource = RZResourceManager::Get().getTextureResource(texHandle);
            //texResource->setCurrentArrayLayer(0);
            //
            if (ImGui::Begin("FrameGraph Debug")) {
                if (ImGui::CollapsingHeader("PBR Deferred Lighting")) {
                    ImGui::SliderFloat("biasScale :", &m_PBRDeferredPass.biasScale, 0.0001f, 0.1f);
                    ImGui::SliderFloat("mxScale :", &m_PBRDeferredPass.maxBias, 0.0001f, 0.1f);
                    static bool visCascades = false;
                    ImGui::Checkbox("Vis Cascaded :", &visCascades);
                    if (visCascades)
                        RZEngine::Get().getWorldSettings().debugFlags |= RendererDebugFlag_VisCSMCascades;
                    else
                        RZEngine::Get().getWorldSettings().debugFlags &= ~RendererDebugFlag_VisCSMCascades;
                }

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

                {
                    RAZIX_PROFILE_SCOPEC("Engine Tools", RZ_PROFILE_COLOR_CORE)

                    // Main menu for Game view tools
                    {
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
                    }

                    // Framegraph resource viewer
                    {
                        if (showResourceViewer) {
#if 1

                            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                            ImGui::SetNextWindowBgAlpha(0.8f);

                            if (ImGui::Begin("[Engine Tool] Frame Graph Resources Viewer")) {
                                // Hacks to rotate text in ImGui. From https://github.com/ocornut/imgui/issues/1286#issue-251214314
                                int  rotation_start_index;
                                auto ImRotateStart = [&]() { rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size; };
                                auto ImRotateEnd   = [&](float rad, ImVec2 center) {
                                    float s = sin(rad), c = cos(rad);
                                    center = ImRotate(center, s, c) - center;

                                    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
                                    for (int i = rotation_start_index; i < buf.Size; i++)
                                        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
                                };

                                i32 passIndex = 0;

                                auto passNodesSize = RZEngine::Get().getWorldRenderer().getFrameGraph().getPassNodesSize();

                                float  passNameHeight    = 400.0f;
                                float  resourceNameWidth = 400.0f;
                                ImVec2 boxSize           = ImVec2(25.0f, ImGui::GetTextLineHeightWithSpacing() * 1.25f);
                                float  width             = (int) passNodesSize * boxSize.x + resourceNameWidth;
                                float  height            = 800;

                                ImGui::BeginChild("Table", ImVec2(width, height));
                                ImDrawList* pCmd = ImGui::GetWindowDrawList();

                                ImVec2 cursor      = ImGui::GetCursorScreenPos();
                                ImVec2 passNamePos = cursor + ImVec2(resourceNameWidth, 0);

                                for (u32 i = 0; i < passNodesSize; i++) {
                                    const auto& passNode         = RZEngine::Get().getWorldRenderer().getFrameGraph().getPassNode(i);
                                    const auto& passNodeName     = passNode.getName();
                                    const auto& passNodeGraphIdx = passNode.getID();

                                    ImRect itemRect(passNamePos + ImVec2(passIndex * boxSize.x, 0.0f), passNamePos + ImVec2((passIndex + 1) * boxSize.x, passNameHeight));
                                    pCmd->AddLine(itemRect.Max, itemRect.Max + ImVec2(0, height), ImColor(1.0f, 1.0f, 1.0f, 0.2f));
                                    ImRotateStart();
                                    ImVec2 size = ImGui::CalcTextSize(passNodeName.c_str());
                                    pCmd->AddText(itemRect.Max - ImVec2(size.x, 0), ImColor(1.0f, 1.0f, 1.0f), passNodeName.c_str());
                                    ImRotateEnd(glm::pi<float>() * 2.2f, itemRect.Max + ImVec2(boxSize.x, 0));
                                    ImGui::ItemAdd(itemRect, passIndex);
                                    bool passActive = ImGui::IsItemHovered();
                                    if (passActive) {
                                        ImGui::BeginTooltip();
                                        {
                                            ImGui::Text("Name           : %s", passNodeName.c_str());
                                            ImGui::Text("Graph ID       : %d", passNodeGraphIdx);
                                            ImGui::Text("Reads          : %zu", passNode.getInputResources().size());
                                            ImGui::Text("Writes         : %zu", passNode.getOutputResources().size());
                                            ImGui::Text("Standalone     : %s", passNode.isStandAlone() ? "true" : "false");
                                            ImGui::Text("Data driven    : %s", passNode.isDataDriven() ? "true" : "false");
                                        }
                                        ImGui::EndTooltip();
                                    }
                                    ++passIndex;
                                }

                                cursor += ImVec2(0.0f, passNameHeight);
                                ImVec2 resourceAccessPos = cursor + ImVec2(resourceNameWidth, 0.0f);

                                //std::unordered_map<GraphicsResource*, int> resourceToIndex;
                                //auto resourceNodes = RZEngine::Get().getWorldRenderer().getFrameGraph().getResourcesNodes();

                                auto resourceNodesSize = RZEngine::Get().getWorldRenderer().getFrameGraph().getResourceNodesSize();

    #if 1
                                for (u32 i = 0; i < resourceNodesSize; i++) {
                                    u32 idx = i;
                                    //resourceNode.getResourceEntryId();
                                    auto& resourceEntry = RZEngine::Get().getWorldRenderer().getFrameGraph().getResourceEntry((Gfx::FrameGraph::RZFrameGraphResource) idx);
                                    //if(resourceEntry.getModel<Graphics::FrameGraph::RZFrameGraphTexture>())

                                    if (resourceEntry.isImported())
                                        continue;

                                    //const auto pFirstPass = resourceEntry.getProducerNode();
                                    //const auto pLastPass  = resourceEntry.getLastNode();
                                    // if (pFirstPass == nullptr || pLastPass == nullptr)
                                    //     continue;

                                    u32 firstPassOffset = idx;
                                    u32 lastPassOffset  = 0;

                                    ImRect itemRect(resourceAccessPos + ImVec2(firstPassOffset * boxSize.x + 1, idx * boxSize.y + 1), resourceAccessPos + ImVec2((lastPassOffset + 1) * boxSize.x - 1, (idx + 1) * boxSize.y - 1));
                                    ImGui::ItemAdd(itemRect, idx);
                                    bool isHovered = ImGui::IsItemHovered();

                                    if (isHovered) {
                                        ImGui::BeginTooltip();
                                        //ImGui::Text("%s", resourceNode.getName());
                                        //
                                        //if (pResource->Type == RGResourceType::Texture) {
                                        //    const TextureDesc& desc = static_cast<const RGTexture*>(pResource)->Desc;
                                        //    ImGui::Text("Res: %dx%dx%d", desc.Width, desc.Height, desc.DepthOrArraySize);
                                        //    ImGui::Text("Fmt: %s", RHI::GetFormatInfo(desc.Format).pName);
                                        //    ImGui::Text("Mips: %d", desc.Mips);
                                        //    ImGui::Text("Size: %s", Math::PrettyPrintDataSize(RHI::GetTextureByteSize(desc.Format, desc.Width, desc.Height, desc.DepthOrArraySize)).c_str());
                                        //} else if (pResource->Type == RGResourceType::Buffer) {
                                        //    const BufferDesc& desc = static_cast<const RGBuffer*>(pResource)->Desc;
                                        //    ImGui::Text("Size: %s", Math::PrettyPrintDataSize(desc.Size).c_str());
                                        //    ImGui::Text("Fmt: %s", RHI::GetFormatInfo(desc.Format).pName);
                                        //    ImGui::Text("Stride: %d", desc.ElementSize);
                                        //    ImGui::Text("Elements: %d", desc.NumElements());
                                        //}
                                        ImGui::EndTooltip();
                                    }

                                    pCmd->AddRectFilled(itemRect.Min, itemRect.Max, ImColor(0.5f, 0.6f, 1.0f));

                                    ImColor boxColor = ImColor(1.0f, 1.0f, 1.0f, 0.5f);

                                    //bool isActivePass = false;
                                    //if (pActivePass) {
                                    //    auto it = std::find_if(pActivePass->Accesses.begin(), pActivePass->Accesses.end(), [pResource](const RGPass::ResourceAccess& access) {
                                    //        return access.pResource == pResource;
                                    //    });
                                    //
                                    //    if (it != pActivePass->Accesses.end()) {
                                    //        isActivePass                         = true;
                                    //        const RGPass::ResourceAccess& access = *it;
                                    //        if (ResourceState::HasWriteResourceState(access.Access))
                                    //            boxColor = ImColor(1.0f, 0.5f, 0.1f, 0.8f);
                                    //        else
                                    //            boxColor = ImColor(0.0f, 0.9f, 0.3f, 0.8f);
                                    //    }
                                    //}

                                    if (isHovered)
                                        pCmd->AddRectFilled(itemRect.Min, itemRect.Max, boxColor);
                                }
    #endif

                                // for (auto& resource: resourceToIndex)
                                //     pCmd->AddText(ImVec2(cursor.x, cursor.y + resource.second * boxSize.y), ImColor(1.0f, 1.0f, 1.0f), resource.first->GetName());

                                ImGui::EndChild();
                            }
                            ImGui::End();
#endif
                            ImGui::PopStyleColor(1);
                        }
                    }

                    // Frame Budgets
                    {
                        if (showBudgets) {
                            if (ImGui::Begin("[Engine Tool] Frame Budgets")) {
                                ImGui::BeginTable("#FrameBudgetsTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
                                {
                                    // Declare columns
                                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide, 150.0f);
                                    ImGui::TableSetupColumn("Time (Ms)", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide, 150.0f);
                                    ImGui::TableSetupColumn("Budget (Ms)", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide, 150.0f);
                                    ImGui::TableSetupColumn("Memory (MiB)", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide, 150.0f);
                                    ImGui::TableSetupColumn("Budget (MiB)", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide, 150.0f);
                                    ImGui::TableHeadersRow();

                                    ImGui::TableNextRow(ImGuiTableRowFlags_None, 20);
                                    ImGui::TableNextColumn();

                                    if (ImGui::TreeNode("Art")) {
                                        ImGui::Indent();
                                        {
                                            ImGui::TableNextRow(ImGuiTableRowFlags_None, 20);
                                            ImGui::TableNextColumn();

                                            ImGui::Text("Depth");
                                            ImGui::TableNextColumn();
                                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.75, 0, 1));
                                            {
                                                ImGui::Text("2.4");
                                            }
                                            ImGui::PopStyleColor(1);
                                            ImGui::TableNextColumn();
                                            ImGui::Text("1.2");
                                        }
                                        ImGui::Unindent();

                                        ImGui::TreePop();
                                    }
                                    ImGui::TableSetColumnIndex(0);
                                    ImGui::TableNextColumn();
                                    ImGui::Text("2.4");
                                    ImGui::TableNextColumn();
                                    ImGui::Text("1.2");
                                    //for (int row = 0; row < 4; row++) {
                                    //    ImGui::TableNextRow();
                                    //    for (int column = 0; column < 3; column++) {
                                    //        ImGui::TableSetColumnIndex(column);
                                    //        ImGui::Text("Row %d Column %d", row, column);
                                    //    }
                                    //}
                                    ImGui::EndTable();
                                }
                            }
                            ImGui::End();
                        }
                    }

                    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                    // Memory Stats
                    {
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
                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
                                ImGui::Text(ICON_FA_CLOCK " current date/time : %s ", std::ctime(&end_time));
                                ImGui::PopStyleColor(1);

                                ImGui::SameLine();

                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
                                std::string engineBuildVersionFull = RazixVersion.getVersionString() + "." + RazixVersion.getReleaseStageString();
                                ImGui::Text("| Engine build version : %s | ", engineBuildVersionFull.c_str());
                                ImGui::PopStyleColor(1);

                                ImGui::SameLine();

                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
                                ImGui::Text(ICON_FA_ID_CARD " project UUID : %s", RZApplication::Get().getProjectUUID().prettyString().c_str());
                                ImGui::PopStyleColor(1);
                            }
                            ImGui::End();
                        }
                    }
                    ImGui::PopStyleVar(1);
                }
            }
        }

        RAZIX_INLINE void RZWorldRenderer::setFrameGraphFilePath(std::string val)
        {
            m_IsFGFilePathDirty  = true;
            m_FrameGraphFilePath = val;
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

                    // This is for when we hot-reload the frame graph
                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto set = Gfx::RHI::Get().getFrameDataSet();
                        if (set)
                            set->Destroy();
                        Gfx::RHI::Get().setFrameDataSet(nullptr);
                    }

                    if (!Gfx::RHI::Get().getFrameDataSet()) {
                        RZDescriptor descriptor{};
                        descriptor.bindingInfo.location.binding = 0;
                        descriptor.bindingInfo.type             = DescriptorType::kUniformBuffer;
                        descriptor.bindingInfo.stage            = ShaderStage(ShaderStage::kGeometry);    // Add support for Pixel shader stage as well
                        descriptor.uniformBuffer                = frameDataBufferHandle;
                        auto m_FrameDataSet                     = RZDescriptorSet::Create({descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Frame Data Set Global"));
                        Gfx::RHI::Get().setFrameDataSet(m_FrameDataSet);
                    }
                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        //--------------------------------------------------------------------------

        void RZWorldRenderer::uploadLightsData(RZScene* scene, RZRendererSettings& settings)
        {
            m_FrameGraph.getBlackboard().add<SceneLightsData>() = m_FrameGraph.addCallbackPass<SceneLightsData>(
                "Pass.Builtin.Code.SceneLightsDataUpload",
                [&](SceneLightsData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

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

                        // Set the Position of the light using this transform component
                        lightComponent.light.getLightData().position = transformComponent.Translation;
                        //lightComponent.light.setDirection(glm::vec3(glm::degrees(transformComponent.Rotation.x), glm::degrees(transformComponent.Rotation.y), glm::degrees(transformComponent.Rotation.z)));
                        lightComponent.light.setDirection(lightComponent.light.getLightData().position);
                        gpuLightsData.lightData[gpuLightsData.numLights] = lightComponent.light.getLightData();

                        gpuLightsData.numLights++;
                    }
                    // update and upload the UBO
                    auto lightsDataBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(data.lightsDataBuffer).getHandle();
                    RZResourceManager::Get().getUniformBufferResource(lightsDataBuffer)->SetData(sizeof(GPULightsData), &gpuLightsData);

                    // This is for when we hot-reload the frame graph
                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto set = Gfx::RHI::Get().getSceneLightsDataSet();
                        if (set)
                            set->Destroy();
                        Gfx::RHI::Get().setSceneLightsDataSet(nullptr);
                    }

                    if (!Gfx::RHI::Get().getSceneLightsDataSet()) {
                        RZDescriptor lightsData_descriptor{};
                        lightsData_descriptor.bindingInfo.location.binding = 0;
                        lightsData_descriptor.bindingInfo.type             = DescriptorType::kUniformBuffer;
                        lightsData_descriptor.bindingInfo.stage            = ShaderStage::kPixel;
                        lightsData_descriptor.uniformBuffer                = lightsDataBuffer;

                        auto m_SceneLightsDataDescriptorSet = RZDescriptorSet::Create({lightsData_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Scene Lights Set Global"));
                        Gfx::RHI::Get().setSceneLightsDataSet(m_SceneLightsDataDescriptorSet);
                    }
                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }
    }    // namespace Gfx
}    // namespace Razix
