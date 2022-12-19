// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZWorldRenderer.h"

#include "Razix/Core/RZMarkers.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

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
            //m_FrameGraph.addCallbackPass(
            //    "ImGui Pass",
            //    [&](FrameGraph::RZFrameGraph::RZBuilder& builder, auto&) {
            //        // Set this as a standalone pass (should not be culled)
            //        builder.setAsStandAlonePass();
            //    },
            //    [=](const auto&, FrameGraph::RZFrameGraphPassResources& resources, void*) {
            //        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            //
            //        //RAZIX_MARK_BEGIN(, "ImGui Pass", glm::vec4(0.85f, 0.65f, 0.0f, 1.0f));
            //
            //        //RAZIX_MARK_END();
            //    });

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