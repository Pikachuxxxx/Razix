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

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZWorldRenderer::buildFrameGraph(RZRendererSettings settings, Razix::RZScene* scene)
        {
            //-------------------------------
            // Grid Pass
            //-------------------------------

            //-------------------------------
            // ImGui Pass
            //-------------------------------
            m_Blackboard.add<RTOnlyPassData>() = m_FrameGraph.addCallbackPass<RTOnlyPassData>(
                "ImGui Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, RTOnlyPassData& data) {
                    builder.setAsStandAlonePass();

                    // Upload to the Blackboard
                    data.outputRT = builder.create<FrameGraph::RZFrameGraphTexture>("ImGui RT", {FrameGraph::TextureType::Texture_RenderTarget, "ImGui RT", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::BGRA8_UNORM});

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
                        {rt, {true, glm::vec4(0.3f, 0.8f, 1.0f, 1.0f)}}};
                    info.extent = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    RZRenderContext::BeginRendering(Graphics::RZRenderContext::getCurrentCommandBuffer(), info);

                    m_ImGuiRenderer.Draw(Graphics::RZRenderContext::getCurrentCommandBuffer());

                    m_ImGuiRenderer.End();

                    // Submit the render queue before presenting next
                    Graphics::RZRenderContext::Submit(Graphics::RZRenderContext::getCurrentCommandBuffer());

                    // Wait on the Presentation done semaphore from the Final Composition pass
                    //auto  waitOnPresentationDoneSemaphore = resources.get<FrameGraph::RZFrameGraphSemaphore>(compositeData.presentationDoneSemaphore).getHandle();
                    // Signal on a semaphore for the Final Composition pass to wait on
                    Graphics::RZRenderContext::SubmitWork(nullptr, resources.get<FrameGraph::RZFrameGraphSemaphore>(data.passDoneSemaphore).getHandle());
                });

            //-------------------------------
            // Final Image Presentation
            //-------------------------------
            m_CompositePass.addPass(m_FrameGraph, m_Blackboard);

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
            m_FrameGraph.execute();
        }
    }    // namespace Graphics
}    // namespace Razix