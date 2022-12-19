// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFinalCompositionPass.h"

#include "Razix/Core/RZApplication.h"

#include "Razix/Graphics/API/RZCommandBuffer.h"
#include "Razix/Graphics/API/RZRenderContext.h"
#include "Razix/Graphics/API/RZSwapchain.h"

#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphRenderTexture.h"

namespace Razix {
    namespace Graphics {

        void RZFinalCompositionPass::addPass(FrameGraph::RZFrameGraph& framegraph, const FrameGraph::RZBlackboard& blackboard)
        {
            // Initialize the resources for the Pass
            init();

            framegraph.addCallbackPass<CompositeData>(
                "Final Composition",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, CompositeData& data) {
                    // Set this as a standalone pass (should not be culled)
                    builder.setAsStandAlonePass();

                    data.presentationTarget = builder.create<FrameGraph::RZFrameGraphRenderTexture>("Present Image", {"Presentation Image", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::RGBA32F});

                    data.depthTexture = builder.create<FrameGraph::RZFrameGraphRenderTexture>("Depth Texture", {"Depth Texture", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::DEPTH});

                    // Writes from this pass
                    data.presentationTarget = builder.write(data.presentationTarget);
                    data.depthTexture       = builder.write(data.depthTexture);
                },
                [=](const CompositeData& data, FrameGraph::RZFrameGraphPassResources& resources, void*) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    // Create the render pass and frame buffer using FrameGraph Resources

                    Graphics::RZRenderContext::AcquireImage();

                    auto cmdBuf = m_CmdBuffers[Graphics::RZRenderContext::getSwapchain()->getCurrentImageIndex()];
                    RZRenderContext::Begin(cmdBuf);

                    // Bind pipeline and stuff
                    m_RenderPass->BeginRenderPass(cmdBuf, glm::vec4(1.0f, 0.85f, 0.0f, 1.0f), m_FrameBuffer, Graphics::SubPassContents{}, );

                    // Bind the pipeline
                    m_Pipeline->Bind(cmdBuf);

                    m_RenderPass->EndRenderPass(cmdBuf);

                    RZRenderContext::Submit(cmdBuf);

                    RZRenderContext::SubmitWork();
                    RZRenderContext::Present();

                    RAZIX_MARK_BEGIN(, "Final Composition", glm::vec4(0.5f));

                    RAZIX_MARK_END();
                });
        }

        void RZFinalCompositionPass::init()
        {
            // Create the pipeline resource for the pass
            // Command Buffers
            for (size_t i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_CmdBuffers[i] = RZCommandBuffer::Create();
                m_CmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Final Composition Pass Command Buffers"));
            }

            // Create the render pass and the attachments
        }
    }    // namespace Graphics
}    // namespace Razix