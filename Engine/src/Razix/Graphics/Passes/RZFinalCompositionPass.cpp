// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFinalCompositionPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/API/RZCommandBuffer.h"
#include "Razix/Graphics/API/RZPipeline.h"
#include "Razix/Graphics/API/RZRenderContext.h"
#include "Razix/Graphics/API/RZSwapchain.h"

#include "Razix/Graphics/Materials/RZMaterial.h"
#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

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

                    data.presentationTarget = builder.create<FrameGraph::RZFrameGraphTexture>("Present Image", {FrameGraph::TextureType::Texture_SwapchainImage, "Presentation Image", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::SCREEN});

                    data.depthTexture = builder.create<FrameGraph::RZFrameGraphTexture>("Depth Texture", {FrameGraph::TextureType::Texture_Depth, "Depth Texture", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::DEPTH});

                    // Writes from this pass
                    data.presentationTarget = builder.write(data.presentationTarget);
                    data.depthTexture       = builder.write(data.depthTexture);

                    /**
                     * Issues:- Well pipeline creation needs a shader and some info from the Frame Graph, so if in a Frame Graph pass
                     * if there are multiple renderables that uses different Materials (aka Shaders) then we can't use the same pipeline.
                     * 
                     * Some solutions:
                     * 
                     * 1. I need to generate multiple pipelines (or PCOs) with all the possible shaders and options and choose them from a cached hash map
                     * 
                     * 2. Lazily create the pipeline during the first frame and cache them, not sure about the overhead of first frame rendering, which should be same as loading phase ig?
                     * Every time a new Material is encountered in the FrameGraph (first time only) it uses the attachment info from the FrameGraph Resources to set the R/W attachments
                     * and creates the pipeline and uses it for the rest of the application life cycle, this also helps when one needs to resize the attachments
                     * 
                     * 3. (Current Workaround) Since the pass is what specifies the shader we can think of Material as a alias for setting the properties on "a" shader, it need not be it's own
                     * It can set it for the current shader that the pass uses
                     */

                    // Build the pipeline here for this pass
                    Graphics::PipelineInfo pipelineInfo{};
                    pipelineInfo.cullMode            = Graphics::CullMode::NONE;
                    pipelineInfo.depthBiasEnabled    = false;
                    pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
                    pipelineInfo.shader              = Graphics::RZShaderLibrary::Get().getShader("composite_pass.rzsf");
                    pipelineInfo.transparencyEnabled = false;
                    pipelineInfo.attachmentFormats   = {RZTexture::Format::SCREEN};

                    m_Pipeline = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Composite Pass Pipeline"));

                    // Init the mesh
                    m_ScreenQuadMesh = Graphics::MeshFactory::CreatePrimitive(Razix::Graphics::MeshPrimitive::ScreenQuad);

                    // FIXME: until we use the new Descriptor Binding API which is resource faced we will do this manual linkage
                    auto setInfos = pipelineInfo.shader->getSetsCreateInfos();
                    for (auto& setInfo: setInfos) {
                        for (auto& descriptor: setInfo.second) {
                            descriptor.texture = Graphics::RZMaterial::GetDefaultTexture();
                        }
                        auto descSet = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Grid Renderer Set"));
                        m_DescriptorSets.push_back(descSet);
                    }
                },
                [=](const CompositeData& data, FrameGraph::RZFrameGraphPassResources& resources, void*) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    Graphics::RZRenderContext::AcquireImage();

                    auto cmdBuf = m_CmdBuffers[Graphics::RZRenderContext::getSwapchain()->getCurrentImageIndex()];
                    RZRenderContext::Begin(cmdBuf);
                    RAZIX_MARK_BEGIN("Final Composition", glm::vec4(0.5f));

                    cmdBuf->UpdateViewport(1280, 720);

                    RenderingInfo info{};
                    info.attachments = {
                        {Graphics::RZRenderContext::getSwapchain()->GetCurrentImage(), {true, glm::vec4(1.0f, 0.8f, 0.0f, 1.0f)}}};
                    info.extent = {1280, 720};

                    RZRenderContext::BeginRendering(cmdBuf, info);

                    // Bind pipeline and stuff
                    m_Pipeline->Bind(cmdBuf);

                    // Bind the descriptor sets
                    Graphics::RZRenderContext::BindDescriptorSets(m_Pipeline, cmdBuf, m_DescriptorSets);

                    // Bind the pipeline
                    m_ScreenQuadMesh->Draw(cmdBuf);

                    RZRenderContext::EndRendering(cmdBuf);

                    RAZIX_MARK_END();
                    RZRenderContext::Submit(cmdBuf);

                    RZRenderContext::SubmitWork();
                    RZRenderContext::Present();
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