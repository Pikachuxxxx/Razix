
// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZWorldRenderer.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#include "Razix/Graphics/FrameGraph/RZBlackboard.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

#include "Razix/Graphics/Lighting/RZIBL.h"

#include "Razix/Graphics/Passes/Data/BRDFData.h"
#include "Razix/Graphics/Passes/Data/FrameBlockData.h"

#include "Razix/Graphics/Renderers/RZDebugRenderer.h"

#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZWorldRenderer::buildFrameGraph(RZRendererSettings& settings, Razix::RZScene* scene)
        {
            // Upload buffers/textures Data to the FrameGraph and GPU initially
            // Upload BRDF look up texture to the GPU
            m_BRDFfLUTTexture                = Graphics::RZTexture2D::CreateFromFile(RZ_DEBUG_NAME_TAG_STR_F_ARG("BRDF LUT") "//RazixContent/Textures/brdf_lut.png", "BRDF LUT");
            m_Blackboard.add<BRDFData>().lut = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("BRDF lut", {FrameGraph::TextureType::Texture_2D, "BRDF lut", {m_BRDFfLUTTexture->getWidth(), m_BRDFfLUTTexture->getHeight()}, {m_BRDFfLUTTexture->getFormat()}}, {m_BRDFfLUTTexture});

            m_NoiseTexture                                        = Graphics::RZTexture2D::CreateFromFile(RZ_DEBUG_NAME_TAG_STR_F_ARG("Noise Texture") "//RazixContent/Textures/volumetric_clouds_noise.png", "Noise Texture", RZTexture::Wrapping::REPEAT);
            m_Blackboard.add<VolumetricCloudsData>().noiseTexture = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("Noise Texture", {FrameGraph::TextureType::Texture_2D, "Noise Texture", {m_NoiseTexture->getWidth(), m_NoiseTexture->getHeight()}, {m_NoiseTexture->getFormat()}}, {m_NoiseTexture});

            // Load the Skybox and Global Light Probes
            // FIXME: This is hard coded make this user land material
#if 1
            m_GlobalLightProbes.skybox   = RZIBL::convertEquirectangularToCubemap("//Textures/HDR/sunset.hdr");
            m_GlobalLightProbes.diffuse  = RZIBL::generateIrradianceMap(m_GlobalLightProbes.skybox);
            m_GlobalLightProbes.specular = RZIBL::generatePreFilteredMap(m_GlobalLightProbes.skybox);
            // Import this into the Frame Graph
            importGlobalLightProbes(m_GlobalLightProbes);
#endif

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

#if 0
            //-------------------------------
            // Cascaded Shadow Maps x
            //-------------------------------
            m_CascadedShadowsRenderer.Init();
            m_CascadedShadowsRenderer.addPass(m_FrameGraph, m_Blackboard, scene, settings);
            //-------------------------------
            // GI - Radiance Pass
            //-------------------------------
            m_GIPass.setGrid(sceneGrid);
            m_GIPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            //-------------------------------
            // GBuffer Pass
            //-------------------------------
            m_GBufferPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            // TODO: will be implemented once proper point lights support is completed
            //-------------------------------
            // [ ] Tiled Lighting Pass
            //-------------------------------

            //-------------------------------
            // [ ] SSAO Pass
            //-------------------------------

            //-------------------------------
            // [...] Deferred Lighting Pass
            //-------------------------------
            m_DeferredPass.setGrid(sceneGrid);
            m_DeferredPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);
#endif

            //-------------------------------
            // [ ] SSR Pass
            //-------------------------------

            //-------------------------------
            // [Test] Simple Shadow map Pass
            //-------------------------------

            m_ShadowRenderer.Init();
            m_ShadowRenderer.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            //-------------------------------
            // [Test] Omni-Dir Shadow Pass
            //-------------------------------

#if 0
            m_Blackboard.add<OmniDirectionalShadowPassData>() = m_FrameGraph.addCallbackPass<OmniDirectionalShadowPassData>(
                "Omni-Directional shadow pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, OmniDirectionalShadowPassData& data) {
                },
                [=](const OmniDirectionalShadowPassData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                });
#endif

            //-------------------------------
            // [Test] Forward Lighting Pass
            //-------------------------------

            auto& frameDataBlock = m_Blackboard.get<FrameData>();
            //const ShadowMapData& cascades       = m_Blackboard.get<ShadowMapData>();
            //auto&                shadowData     = m_Blackboard.get<SimpleShadowPassData>();

#if 1
            m_Blackboard.add<SceneData>() = m_FrameGraph.addCallbackPass<SceneData>(
                "Forward Lighting Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, SceneData& data) {
                    builder.setAsStandAlonePass();

                    data.outputHDR = builder.create<FrameGraph::RZFrameGraphTexture>("Scene HDR color", {FrameGraph::TextureType::Texture_RenderTarget, "Scene HDR color", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::RGBA32F});

                    data.depth = builder.create<FrameGraph::RZFrameGraphTexture>("Scene Depth", {FrameGraph::TextureType::Texture_Depth, "Scene Depth", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::DEPTH16_UNORM});

                    data.outputHDR = builder.write(data.outputHDR);
                    data.depth     = builder.write(data.depth);

                    builder.read(frameDataBlock.frameData);

                    //builder.read(shadowData.shadowMap);
                    //builder.read(shadowData.lightVP);

                    m_ForwardRenderer.Init();
                },
                [=](const SceneData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    m_ForwardRenderer.Begin(scene);

                    auto rtHDR = resources.get<FrameGraph::RZFrameGraphTexture>(data.outputHDR).getHandle();
                    auto dt    = resources.get<FrameGraph::RZFrameGraphTexture>(data.depth).getHandle();

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {rtHDR, {true, scene->getSceneCamera().getBgColor()}},
                    };
                    info.depthAttachment = {dt, {true, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)}};
                    info.extent          = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize          = true;

                    // Set the Descriptor Set once rendering starts
                    static bool updatedSets = false;
                    if (!updatedSets) {
                        auto frameDataBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(frameDataBlock.frameData).getHandle();

                        RZDescriptor frame_descriptor{};
                        frame_descriptor.offset              = 0;
                        frame_descriptor.size                = sizeof(GPUFrameData);
                        frame_descriptor.bindingInfo.binding = 0;
                        frame_descriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        frame_descriptor.bindingInfo.stage   = ShaderStage::VERTEX;
                        frame_descriptor.uniformBuffer       = frameDataBuffer;

                        m_ForwardRenderer.SetFrameDataHeap(RZDescriptorSet::Create({frame_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Frame Data Buffer Forward")));

    #if 0
                        auto csmTextures = resources.get<FrameGraph::RZFrameGraphTexture>(shadowData.shadowMap).getHandle();

                        RZDescriptor csm_descriptor{};
                        csm_descriptor.bindingInfo.binding = 0;
                        csm_descriptor.bindingInfo.type    = DescriptorType::IMAGE_SAMPLER;
                        csm_descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        csm_descriptor.texture             = csmTextures;

                        RZDescriptor shadow_data_descriptor{};
                        shadow_data_descriptor.size                = sizeof(ShadowMapData);
                        shadow_data_descriptor.bindingInfo.binding = 1;
                        shadow_data_descriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        shadow_data_descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        shadow_data_descriptor.uniformBuffer       = resources.get<FrameGraph::RZFrameGraphBuffer>(shadowData.lightVP).getHandle();
    #endif

                        m_ForwardRenderer.setCSMArrayHeap(RZDescriptorSet::Create({ /*csm_descriptor, shadow_data_descriptor*/ } RZ_DEBUG_NAME_TAG_STR_E_ARG("CSM + Matrices")));

                        updatedSets = true;
                    }

                    RHI::BeginRendering(Graphics::RHI::GetCurrentCommandBuffer(), info);

                    m_ForwardRenderer.Draw(Graphics::RHI::GetCurrentCommandBuffer());

                    m_ForwardRenderer.End();

                    Graphics::RHI::Submit(Graphics::RHI::GetCurrentCommandBuffer());

                    Graphics::RHI::SubmitWork({}, {});
                });
#endif
            //-------------------------------
            // PBR Pass
            //-------------------------------
            //m_PBRLightingPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);
            SceneData sceneData = m_Blackboard.get<SceneData>();

            //-------------------------------
            // [x] Skybox Pass
            //-------------------------------
            //m_SkyboxPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            //-------------------------------
            // [x] Bloom Pass
            //-------------------------------
            if (settings.renderFeatures & RendererFeature_Bloom)
                m_BloomPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            //-------------------------------
            // Debug Scene Pass
            //-------------------------------
            m_FrameGraph.addCallbackPass(
                "Debug Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, auto& data) {
                    builder.setAsStandAlonePass();

                    RZDebugRenderer::Get()->Init();

                    builder.read(sceneData.outputHDR);
                    builder.read(sceneData.depth);
                    builder.read(frameDataBlock.frameData);
                },
                [=](const auto& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
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
                        if (lightComponent.light.getType() == LightType::Point)
                            RZDebugRenderer::DrawLight(&lights[0].light, glm::vec4(0.8f, 0.65f, 0.0f, 1.0f));
                    }

                    RZDebugRenderer::Get()->Begin(scene);

                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.outputHDR).getHandle();
                    auto dt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.depth).getHandle();

                    RenderingInfo info{};
                    info.colorAttachments = {{rt, {false, scene->getSceneCamera().getBgColor()}}};
                    info.depthAttachment  = {dt, {false, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)}};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize           = true;

                    RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                    static bool updatedSets = false;
                    if (!updatedSets) {
                        auto frameDataBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(frameDataBlock.frameData).getHandle();

                        RZDescriptor frame_descriptor{};
                        frame_descriptor.offset              = 0;
                        frame_descriptor.size                = sizeof(GPUFrameData);
                        frame_descriptor.bindingInfo.binding = 0;
                        frame_descriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        frame_descriptor.bindingInfo.stage   = ShaderStage::VERTEX;
                        frame_descriptor.uniformBuffer       = frameDataBuffer;

                        RZDebugRenderer::Get()->SetFrameDataHeap(RZDescriptorSet::Create({frame_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Frame Data Buffer Debug")));
                        updatedSets = true;
                    }

                    RZDebugRenderer::Get()->Draw(RHI::GetCurrentCommandBuffer());

                    RHI::EndRendering(Graphics::RHI::GetCurrentCommandBuffer());

                    RZDebugRenderer::Get()->End();

                    Graphics::RHI::Submit(Graphics::RHI::GetCurrentCommandBuffer());
                    Graphics::RHI::SubmitWork({}, {});
                });

            //-------------------------------
            // ImGui Pass
            //-------------------------------
            m_Blackboard.add<RTOnlyPassData>() = m_FrameGraph.addCallbackPass<RTOnlyPassData>(
                "ImGui Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, RTOnlyPassData& data) {
                    builder.setAsStandAlonePass();

                    data.passDoneSemaphore = builder.create<FrameGraph::RZFrameGraphSemaphore>("ImGui Pass Signal Semaphore", {"ImGui Pass Semaphore"});

                    data.outputRT          = builder.write(sceneData.outputHDR);
                    data.passDoneSemaphore = builder.write(data.passDoneSemaphore);

                    builder.read(sceneData.depth);

                    m_ImGuiRenderer.Init();
                },
                [=](const RTOnlyPassData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    m_ImGuiRenderer.Begin(scene);

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(data.outputRT).getHandle();
                    auto dt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.depth).getHandle();

                    RenderingInfo info{};
                    info.colorAttachments = {{rt, {false, glm::vec4(0.0f)}}};
                    info.depthAttachment  = {dt, {false, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)}};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize           = true;

                    RHI::BeginRendering(Graphics::RHI::GetCurrentCommandBuffer(), info);

                    m_ImGuiRenderer.Draw(Graphics::RHI::GetCurrentCommandBuffer());

                    m_ImGuiRenderer.End();

                    // Submit the render queue before presenting next
                    Graphics::RHI::Submit(Graphics::RHI::GetCurrentCommandBuffer());

                    // Signal on a semaphore for the next pass (Final Composition pass) to wait on
                    Graphics::RHI::SubmitWork({}, {resources.get<FrameGraph::RZFrameGraphSemaphore>(data.passDoneSemaphore).getHandle()});
                });

            //-------------------------------
            // Final Image Presentation
            //-------------------------------
            m_CompositePass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            // Compile the Frame Graph
            RAZIX_CORE_INFO("Compiling FrameGraph ....");
            m_FrameGraph.compile();

            // Dump the Frame Graph for visualization
            std::string outPath;
            RZVirtualFileSystem::Get().resolvePhysicalPath("//RazixContent/FrameGraphs", outPath, true);
            RAZIX_CORE_INFO("Exporting FrameGraph .... to ({0})", outPath);
            std::ofstream os(outPath + "/pbr_lighting_test.dot");
            os << m_FrameGraph;
        }

        void RZWorldRenderer::drawFrame(RZRendererSettings& settings, Razix::RZScene* scene)
        {
            m_CascadedShadowsRenderer.updateCascades(scene);

            m_FrameGraph.execute(nullptr, &m_TransientResources);
        }

        void RZWorldRenderer::destroy()
        {
            // Destroy Imported Resources
            m_BRDFfLUTTexture->Release(true);
            m_NoiseTexture->Release(true);

#if 1
            m_GlobalLightProbes.skybox->Release(true);
            m_GlobalLightProbes.diffuse->Release(true);
            m_GlobalLightProbes.specular->Release(true);

#endif
            // Destroy Renderers
            //m_ForwardRenderer.Destroy();
            m_CascadedShadowsRenderer.Destroy();
            m_ShadowRenderer.Destroy();
            m_ImGuiRenderer.Destroy();
            RZDebugRenderer::Get()->Destroy();

            // Destroy Passes
            m_PBRLightingPass.destroy();
            m_SkyboxPass.destroy();
            m_CompositePass.destroy();
            //m_GIPass.destroy();
            //m_GBufferPass.destroy();

            // Destroy Frame Graph Resources
            m_TransientResources.destroyResources();
        }

        void RZWorldRenderer::importGlobalLightProbes(LightProbe globalLightProbe)
        {
            auto& globalLightProbeData = m_Blackboard.add<GlobalLightProbeData>();

            globalLightProbeData.environmentMap = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("Environment Map", {FrameGraph::TextureType::Texture_CubeMap, "Environment Map", {globalLightProbe.skybox->getWidth(), globalLightProbe.skybox->getHeight()}, {globalLightProbe.skybox->getFormat()}}, {globalLightProbe.skybox});

            globalLightProbeData.diffuseIrradianceMap = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("Diffuse Irradiance", {FrameGraph::TextureType::Texture_CubeMap, "Diffuse Irradiance", {globalLightProbe.diffuse->getWidth(), globalLightProbe.diffuse->getHeight()}, {globalLightProbe.diffuse->getFormat()}}, {globalLightProbe.diffuse});

            globalLightProbeData.specularPreFilteredMap = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("Specular PreFiltered", {FrameGraph::TextureType::Texture_CubeMap, "Specular PreFiltered", {globalLightProbe.specular->getWidth(), globalLightProbe.specular->getHeight()}, {globalLightProbe.specular->getFormat()}}, {globalLightProbe.specular});
        }

        //--------------------------------------------------------------------------

        void RZWorldRenderer::uploadFrameData(RZScene* scene, RZRendererSettings& settings)
        {
            m_Blackboard.add<FrameData>() = m_FrameGraph.addCallbackPass<FrameData>(
                "Frame Data Upload",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, FrameData& data) {
                    builder.setAsStandAlonePass();

                    data.frameData = builder.create<FrameGraph::RZFrameGraphBuffer>("Frame Data", {"FrameData", sizeof(GPUFrameData)});

                    data.frameData = builder.write(data.frameData);
                },
                [=](const FrameData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    GPUFrameData gpuData{};
                    gpuData.time += gpuData.deltaTime;
                    gpuData.deltaTime      = RZEngine::Get().GetStatistics().DeltaTime;
                    gpuData.resolution     = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    gpuData.debugFlags     = settings.debugFlags;
                    gpuData.renderFeatures = settings.renderFeatures;

                    auto& sceneCam = scene->getSceneCamera();

                    sceneCam.setAspectRatio(f32(RZApplication::Get().getWindow()->getWidth()) / f32(RZApplication::Get().getWindow()->getHeight()));
#if 0
                    // Test code to view from the Directional Light POV to configure shadow map
                    auto      lights     = scene->GetComponentsOfType<LightComponent>();
                    auto&     dir_light  = lights[0].light;
                    glm::mat4 lightView  = glm::lookAt(dir_light.getPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    float     near_plane = -100.0f, far_plane = 50.0f;
                    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -far_plane * 2.0f, far_plane);
#endif

                    gpuData.camera.projection         = sceneCam.getProjection();
                    gpuData.camera.inversedProjection = glm::inverse(gpuData.camera.projection);
                    gpuData.camera.view               = sceneCam.getViewMatrix();
                    gpuData.camera.inversedView       = glm::inverse(gpuData.camera.view);
                    gpuData.camera.fov                = sceneCam.getPerspectiveVerticalFOV();
                    gpuData.camera.nearPlane          = sceneCam.getPerspectiveNearClip();
                    gpuData.camera.farPlane           = sceneCam.getPerspectiveFarClip();

                    // update and upload the UBO
                    resources.get<FrameGraph::RZFrameGraphBuffer>(data.frameData).getHandle()->SetData(sizeof(GPUFrameData), &gpuData);
                });
        }

        //--------------------------------------------------------------------------

        void RZWorldRenderer::uploadLightsData(RZScene* scene, RZRendererSettings& settings)
        {
            m_Blackboard.add<SceneLightsData>() = m_FrameGraph.addCallbackPass<SceneLightsData>(
                "Scene Lights Data Upload",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, SceneLightsData& data) {
                    builder.setAsStandAlonePass();

                    data.lightsDataBuffer = builder.create<FrameGraph::RZFrameGraphBuffer>("Scene Lights Data", {"Scene Lights Data", sizeof(GPULightsData)});
                    data.lightsDataBuffer = builder.write(data.lightsDataBuffer);
                },
                [=](const SceneLightsData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    GPULightsData gpuLightsData{};

                    auto& registry = scene->getRegistry();

                    // Upload the lights data after updating some stuff such as position etc.
                    auto group = scene->getRegistry().group<LightComponent>(entt::get<TransformComponent>);
                    for (auto entity: group) {
                        const auto& [lightComponent, transformComponent] = group.get<LightComponent, TransformComponent>(entity);

                        // Set the Position of the light using this transform component
                        lightComponent.light.getLightData().position = transformComponent.Translation;
                        lightComponent.light.setDirection(glm::vec3(glm::degrees(transformComponent.Rotation.x), glm::degrees(transformComponent.Rotation.y), glm::degrees(transformComponent.Rotation.z)));
                        gpuLightsData.lightData[gpuLightsData.numLights] = lightComponent.light.getLightData();

                        gpuLightsData.numLights++;
                    }
                    // update and upload the UBO
                    resources.get<FrameGraph::RZFrameGraphBuffer>(data.lightsDataBuffer).getHandle()->SetData(sizeof(GPUFrameData), &gpuLightsData);
                });
        }
    }    // namespace Graphics
}    // namespace Razix