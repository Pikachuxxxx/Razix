// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZWorldRenderer.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/FrameGraph/RZBlackboard.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

#include "Razix/Graphics/Lighting/RZIBL.h"

#include "Razix/Graphics/Passes/Data/BRDFData.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZWorldRenderer::buildFrameGraph(RZRendererSettings settings, Razix::RZScene* scene)
        {
            // Upload buffers/textures Data to the FrameGraph and GPU initially
            // Upload BRDF look up texture to the GPU
            brdfLUTTexture                   = Graphics::RZTexture2D::CreateFromFile(RZ_DEBUG_NAME_TAG_STR_F_ARG("BRDF LUT") "//RazixContent/Textures/brdf_lut.png", "BRDF LUT");
            m_Blackboard.add<BRDFData>().lut = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("BRDF lut", {FrameGraph::TextureType::Texture_2D, "BRDF lut", {brdfLUTTexture->getWidth(), brdfLUTTexture->getHeight()}, {brdfLUTTexture->getFormat()}}, {brdfLUTTexture});

            RZIBL::convertEquirectangularToCubemap("//Textures/HDR/newport_loft.hdr");

            //-------------------------------
            //
            //-------------------------------

            //-------------------------------
            // ImGui Pass
            //-------------------------------
            if (settings.renderFeatures & RendererFeature_ImGui) {
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
                        m_ImGuiRenderer.Begin(scene);

                        auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(data.outputRT).getHandle();

                        RenderingInfo info{};
                        info.attachments = {
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
            }

            //-------------------------------
            // Final Image Presentation
            //-------------------------------
            m_CompositePass.addPass(m_FrameGraph, m_Blackboard, settings);

            // Compile the Frame Graph
            m_FrameGraph.compile();

            // Dump the Frame Graph for visualization
            std::string outPath;
            RZVirtualFileSystem::Get().resolvePhysicalPath("//RazixContent/FrameGraphs", outPath, true);
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
            brdfLUTTexture->Release(true);

            m_ImGuiRenderer.Destroy();
            m_CompositePass.destoy();
            m_TransientResources.destroyResources();
        }
    }    // namespace Graphics
}    // namespace Razix