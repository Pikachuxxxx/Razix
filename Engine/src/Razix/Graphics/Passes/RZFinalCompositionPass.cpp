// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFinalCompositionPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Passes/Data/GlobalData.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZSwapchain.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/API/Data/RZPipelineData.h"

#include "Razix/Graphics/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

namespace Razix {
    namespace Graphics {

        void RZFinalCompositionPass::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, RZScene* scene, RZRendererSettings& settings)
        {
            DescriptorSetsCreateInfos setInfos;

            RZPipelineDesc pipelineInfo{
                // Build the pipeline here for this pass
                .shader                 = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Composition),
                .colorAttachmentFormats = {RZTextureProperties::Format::BGRA8_UNORM},
                .cullMode               = Graphics::CullMode::NONE,
                .drawType               = Graphics::DrawType::TRIANGLE,
                .transparencyEnabled    = true,
                .depthBiasEnabled       = false};

            // Get the final Scene Color HDR RT
            SceneData sceneData = blackboard.get<SceneData>();

#if 1
            blackboard.add<CompositeData>() = framegraph.addCallbackPass<CompositeData>(
                "Final Composition",
                [&](CompositeData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    // Set this as a standalone pass (should not be culled)
                    builder.setAsStandAlonePass();

                    RZTextureDesc presentImageDesc{
                        .name   = "Present Image",
                        .width  = RZApplication::Get().getWindow()->getWidth(),
                        .height = RZApplication::Get().getWindow()->getHeight(),
                        .type   = RZTextureProperties::Type::Texture_2D,
                        .format = RZTextureProperties::Format::BGRA8_UNORM};

                    RZTextureDesc depthImageDesc{
                        .name       = "Depth Image",
                        .width      = RZApplication::Get().getWindow()->getWidth(),
                        .height     = RZApplication::Get().getWindow()->getHeight(),
                        .type       = RZTextureProperties::Type::Texture_Depth,
                        .format     = RZTextureProperties::Format::DEPTH32F,
                        .filtering  = {RZTextureProperties::Filtering::FilterMode::NEAREST, RZTextureProperties::Filtering::FilterMode::NEAREST},
                        .enableMips = false};

                    data.presentationTarget = builder.create<FrameGraph::RZFrameGraphTexture>("Present Image", CAST_TO_FG_TEX_DESC presentImageDesc);

                    data.depthTexture = builder.create<FrameGraph::RZFrameGraphTexture>("Depth Texture", (FrameGraph::RZFrameGraphTexture::Desc) depthImageDesc);

                    // Writes from this pass
                    data.presentationTarget = builder.write(data.presentationTarget);
                    data.depthTexture       = builder.write(data.depthTexture);

                    if (settings.renderFeatures & RendererFeature_ImGui) {
                        builder.read(sceneData.outputHDR);
                        builder.read(sceneData.depth);
                    }

                    /**
                     * Issues:- Well pipeline creation needs a shader and some info from the Frame Graph(all the output attachments that the current frame graph pas writes to)
                     * so if in a Frame Graph pass if there are multiple renderables that uses different Materials (and diff Shaders) then we can't use the same pipeline.
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

                    m_Pipeline = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Composite Pass Pipeline"));

                    // Init the mesh
                    m_ScreenQuadMesh = Graphics::MeshFactory::CreatePrimitive(Razix::Graphics::MeshPrimitive::ScreenQuad);
                },
                [=](const CompositeData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_MARK_BEGIN("Final Composition", glm::vec4(0.5f));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    cmdBuffer->UpdateViewport(RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight());

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {Graphics::RHI::GetSwapchain()->GetCurrentImage(), {true, glm::vec4(0.2f)}} /*,
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.depthTexture).getHandle(), {true}}*/
                    };
                    info.extent = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize = true;

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    m_Pipeline->Bind(cmdBuffer);

                    Graphics::RHI::BindDescriptorSet(m_Pipeline, cmdBuffer, RHI::Get().getFrameDataSet(), BindingTable_System::SET_IDX_FRAME_DATA);
                    RHI::EnableBindlessTextures(m_Pipeline, cmdBuffer);

                    m_ScreenQuadMesh->getVertexBuffer()->Bind(cmdBuffer);
                    m_ScreenQuadMesh->getIndexBuffer()->Bind(cmdBuffer);

                    u32            idx = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.outputHDR).getHandle().getIndex();
                    RZPushConstant pc;
                    pc.size        = sizeof(u32);
                    pc.data        = &idx;
                    pc.shaderStage = ShaderStage::PIXEL;
                    RHI::BindPushConstant(m_Pipeline, cmdBuffer, pc);

                    // No need to bind the mesh material

                    RHI::DrawIndexed(cmdBuffer, m_ScreenQuadMesh->getIndexCount(), 1, 0, 0, 0);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                });
#endif
        }

        void RZFinalCompositionPass::destroy()
        {
            m_Pipeline->Destroy();
            m_ScreenQuadMesh->Destroy();
        }
    }    // namespace Graphics
}    // namespace Razix