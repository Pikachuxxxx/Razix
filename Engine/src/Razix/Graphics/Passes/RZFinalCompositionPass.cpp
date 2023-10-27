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
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZSwapchain.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/API/Data/RZPipelineData.h"

#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

namespace Razix {
    namespace Graphics {

        void RZFinalCompositionPass::addPass(FrameGraph::RZFrameGraph& framegraph, RZScene* scene, RZRendererSettings& settings)
        {
            DescriptorsPerHeapMap setInfos;

            RZPipelineDesc pipelineInfo{
                // Build the pipeline here for this pass
                .name                   = "Composition Pipeline",
                .shader                 = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Composition),
                .colorAttachmentFormats = {TextureFormat::SCREEN},
                .cullMode               = Graphics::CullMode::None,
                .drawType               = Graphics::DrawType::Triangle,
                .transparencyEnabled    = true,
                .depthBiasEnabled       = false};

            // Get the final output
            FrameGraph::RZFrameGraphResource FinalOutputRenderTarget = framegraph.getBlackboard().getFinalOutputID();
#if 1
            framegraph.getBlackboard().add<CompositeData>() = framegraph.addCallbackPass<CompositeData>(
                "Pass.Builtin.Code.FinalComposition",
                [&](CompositeData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    // Set this as a standalone pass (should not be culled)
                    builder.setAsStandAlonePass();

                    RZTextureDesc presentImageDesc{
                        .name   = "Present Image",
                        .width  = RZApplication::Get().getWindow()->getWidth(),
                        .height = RZApplication::Get().getWindow()->getHeight(),
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::SCREEN};

                    data.presentationTarget = builder.create<FrameGraph::RZFrameGraphTexture>("Present Image", CAST_TO_FG_TEX_DESC presentImageDesc);

                    // Writes from this pass
                    data.presentationTarget = builder.write(data.presentationTarget);

                    // Read the Final RT from where ever it's given from
                    builder.read(FinalOutputRenderTarget);

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

                    m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

                    // Init the mesh
                    m_ScreenQuadMesh = Graphics::MeshFactory::CreatePrimitive(Razix::Graphics::MeshPrimitive::ScreenQuad);

                    setInfos = RZResourceManager::Get().getShaderResource(pipelineInfo.shader)->getDescriptorsPerHeapMap();
                    for (auto& setInfo: setInfos) {
                        for (auto& descriptor: setInfo.second) {
                            descriptor.texture = Graphics::RZMaterial::GetDefaultTexture();
                        }
                        m_DescriptorSets = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Composite Set"), true);
                    }
                },
                [=](const CompositeData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_MARK_BEGIN("Final Composition", glm::vec4(0.5f));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    // Update the Descriptor Set with the new texture once

                    if (!updatedRT) {
                        auto setInfos = RZResourceManager::Get().getShaderResource(pipelineInfo.shader)->getDescriptorsPerHeapMap();
                        for (auto& setInfo: setInfos) {
                            for (auto& descriptor: setInfo.second) {
                                // change the layout to be in Shader Read Only Optimal
                                descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(FinalOutputRenderTarget).getHandle();
                            }
                            m_DescriptorSets->UpdateSet(setInfo.second);
                        }
                        updatedRT = true;
                    }

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {
                        {Graphics::RHI::GetSwapchain()->GetCurrentImage(), {true, ClearColorPresets::TransparentBlack}}};
                    info.resize = true;

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    //Graphics::RHI::BindDescriptorSet(m_Pipeline, cmdBuffer, RHI::Get().getFrameDataSet(), BindingTable_System::SET_IDX_FRAME_DATA);
                    Graphics::RHI::BindDescriptorSet(m_Pipeline, RHI::GetCurrentCommandBuffer(), m_DescriptorSets, BindingTable_System::SET_IDX_FRAME_DATA);
                    //RHI::EnableBindlessTextures(m_Pipeline, cmdBuffer);

                    m_ScreenQuadMesh->getVertexBuffer()->Bind(cmdBuffer);
                    m_ScreenQuadMesh->getIndexBuffer()->Bind(cmdBuffer);

                    struct PCData
                    {
                        u32       tonemapMode;
                        glm::vec2 screenRes;
                    } pcData{};
                    pcData.tonemapMode = m_TonemapMode;
                    pcData.screenRes   = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};

                    RZPushConstant pc;
                    pc.size        = sizeof(u32);
                    pc.data        = &pcData;
                    pc.shaderStage = ShaderStage::Pixel;
                    RHI::BindPushConstant(m_Pipeline, cmdBuffer, pc);

                    // No need to bind the mesh material
                    RHI::DrawIndexed(cmdBuffer, m_ScreenQuadMesh->getIndexCount(), 1, 0, 0, 0);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                },
                [=](FrameGraph::RZPassResourceDirectory& resources, u32 width, u32 height) {
                    updatedRT = false;
                    //auto setInfos = pipelineInfo.shader->getSetsCreateInfos();
                    //for (auto& setInfo: setInfos) {
                    //    for (auto& descriptor: setInfo.second) {
                    //        // change the layout to be in Shader Read Only Optimal
                    //        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.outputHDR).getHandle();
                    //    }
                    //    m_DescriptorSets->UpdateSet(setInfo.second);
                    //}
                    RZGraphicsContext::GetContext()->Wait();
                });
#endif
        }

        void RZFinalCompositionPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            m_ScreenQuadMesh->Destroy();
        }
    }    // namespace Graphics
}    // namespace Razix