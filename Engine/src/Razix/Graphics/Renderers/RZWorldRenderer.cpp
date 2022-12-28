// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZWorldRenderer.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#include "Razix/Graphics/FrameGraph/RZBlackboard.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

#include "Razix/Graphics/Lighting/RZIBL.h"

#include "Razix/Graphics/Passes/Data/BRDFData.h"
#include "Razix/Graphics/Passes/Data/GlobalLightProbeData.h"

#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZWorldRenderer::buildFrameGraph(RZRendererSettings settings, Razix::RZScene* scene)
        {
            // Upload buffers/textures Data to the FrameGraph and GPU initially
            // Upload BRDF look up texture to the GPU
            m_BRDFfLUTTexture                = Graphics::RZTexture2D::CreateFromFile(RZ_DEBUG_NAME_TAG_STR_F_ARG("BRDF LUT") "//RazixContent/Textures/brdf_lut.png", "BRDF LUT");
            m_Blackboard.add<BRDFData>().lut = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("BRDF lut", {FrameGraph::TextureType::Texture_2D, "BRDF lut", {m_BRDFfLUTTexture->getWidth(), m_BRDFfLUTTexture->getHeight()}, {m_BRDFfLUTTexture->getFormat()}}, {m_BRDFfLUTTexture});

            // Load the Skybox and Global Light Probes
#if 0
            m_Skybox                     = RZIBL::convertEquirectangularToCubemap("//Textures/HDR/newport_loft.hdr");
            m_GlobalLightProbes.diffuse  = RZIBL::generateIrradianceMap(m_Skybox);
            m_GlobalLightProbes.specular = RZIBL::generatePreFilteredMap(m_Skybox);
            // Import this into the Frame Graph
            importGlobalLightProbes(m_GlobalLightProbes);
#endif

            // Cull Lights (Directional + Point) on CPU against camera Frustum First
            // TODO: Get the list of lights in the scene and cull them against the camera frustum and disable ActiveComponent for culled lights, but for now we can just ignore that
            auto                 group = scene->getRegistry().group<LightComponent>(entt::get<ActiveComponent>);
            std::vector<RZLight> sceneLights;
            for (auto& entity: group)
                sceneLights.push_back(group.get<LightComponent>(entity).light);

            // Pass the Scene AABB and Grid info for GI + Tiled lighting
            // TODO: Make this dynamic as scene glows larger
            m_SceneAABB = {glm::vec3(-76.83, -5.05, -47.31), glm::vec3(71.99, 57.17, 44.21)};
            const Maths::RZGrid sceneGrid(m_SceneAABB);

#if 1
            //-------------------------------
            // Cascaded Shadow Maps
            //-------------------------------
            m_CascadedShadowsRenderer.Init();
            m_CascadedShadowsRenderer.addPass(m_FrameGraph, m_Blackboard, scene, settings);
#endif

            //-------------------------------
            // GI - Radiance Pass
            //-------------------------------
            m_GIPass.setGrid(sceneGrid);
            m_GIPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

#if 1
            //-------------------------------
            // ImGui Pass
            //-------------------------------
            m_Blackboard.add<RTOnlyPassData>() = m_FrameGraph.addCallbackPass<RTOnlyPassData>(
                "ImGui Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, RTOnlyPassData& data) {
                    builder.setAsStandAlonePass();

                    // Upload to the Blackboard
                    data.outputRT = builder.create<FrameGraph::RZFrameGraphTexture>("ImGui RT", {FrameGraph::TextureType::Texture_RenderTarget, "ImGui RT", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::RGBA32});

                    data.passDoneSemaphore = builder.create<FrameGraph::RZFrameGraphSemaphore>("ImGui Pass Signal Semaphore", {"ImGui Pass Semaphore"});

                    data.outputRT          = builder.write(data.outputRT);
                    data.passDoneSemaphore = builder.write(data.passDoneSemaphore);

                    m_ImGuiRenderer.Init();
                },
                [=](const RTOnlyPassData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RZGraphicsContext::GetContext()->Wait();

                    m_ImGuiRenderer.Begin(scene);

                    struct CheckpointData
                    {
                        std::string RenderPassName = "ImGui Pass";
                    } checkpointData;

                    RZRenderContext::SetCmdCheckpoint(Graphics::RZRenderContext::getCurrentCommandBuffer(), &checkpointData);

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(data.outputRT).getHandle();

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {rt, {true, glm::vec4(0.0f)}}};
                    info.extent = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    RZRenderContext::BeginRendering(Graphics::RZRenderContext::getCurrentCommandBuffer(), info);

                    m_ImGuiRenderer.Draw(Graphics::RZRenderContext::getCurrentCommandBuffer());

                    m_ImGuiRenderer.End();

                    // Submit the render queue before presenting next
                    Graphics::RZRenderContext::Submit(Graphics::RZRenderContext::getCurrentCommandBuffer());

                    // Signal on a semaphore for the next pass (Final Composition pass) to wait on
                    Graphics::RZRenderContext::SubmitWork({}, {resources.get<FrameGraph::RZFrameGraphSemaphore>(data.passDoneSemaphore).getHandle()});
                });

            //-------------------------------
            // Final Image Presentation
            //-------------------------------
            m_CompositePass.addPass(m_FrameGraph, m_Blackboard, scene, settings);
#endif

            // Compile the Frame Graph
            RAZIX_CORE_INFO("Compiling FrameGraph ....");
            m_FrameGraph.compile();

            // Dump the Frame Graph for visualization
            std::string outPath;
            RZVirtualFileSystem::Get().resolvePhysicalPath("//RazixContent/FrameGraphs", outPath, true);
            RAZIX_CORE_INFO("Exporting FrameGraph .... to ({0})", outPath);
            std::ofstream os(outPath + "/frame_graph_test.dot");
            os << m_FrameGraph;
        }

        void RZWorldRenderer::drawFrame(RZRendererSettings settings, Razix::RZScene* scene)
        {
            // TODO: Since the Render Context is a singleton we don't need it, so remove it from the API
            m_FrameGraph.execute(nullptr, &m_TransientResources);
        }

        void RZWorldRenderer::destroy()
        {
            // Destroy Imported Resources
            m_BRDFfLUTTexture->Release(true);

#if 0
            m_Skybox->Release(true);
            m_GlobalLightProbes.diffuse->Release(true);
            m_GlobalLightProbes.specular->Release(true);
#endif

            // Destroy Renderers
            m_CascadedShadowsRenderer.Destroy();
            m_ImGuiRenderer.Destroy();

            // Destroy Passes
            m_CompositePass.destroy();

            // Destroy Frame Graph Resources
            m_TransientResources.destroyResources();
        }

        void RZWorldRenderer::importGlobalLightProbes(GlobalLightProbe globalLightProbe)
        {
            auto& globalLightProbeData = m_Blackboard.add<GlobalLightProbeData>();

            globalLightProbeData.diffuseIrradianceMap = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("Diffuse Irradiance", {FrameGraph::TextureType::Texture_CubeMap, "Diffuse Irradiance", {globalLightProbe.diffuse->getWidth(), globalLightProbe.diffuse->getHeight()}, {globalLightProbe.diffuse->getFormat()}}, {globalLightProbe.diffuse});

            globalLightProbeData.specularPreFilteredMap = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("Specular PreFiltered", {FrameGraph::TextureType::Texture_CubeMap, "Specular PreFiltered", {globalLightProbe.specular->getWidth(), globalLightProbe.specular->getHeight()}, {globalLightProbe.specular->getFormat()}}, {globalLightProbe.specular});
        }
    }    // namespace Graphics
}    // namespace Razix