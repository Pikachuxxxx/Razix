// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZCompositionPass.h"

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

        void RZCompositionPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            auto compositionShader = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Composition);

            RZPipelineDesc pipelineInfo{
                // Build the pipeline here for this pass
                .name                   = "Pipeline.Composition",
                .shader                 = compositionShader,
                .colorAttachmentFormats = {TextureFormat::SCREEN},
                .cullMode               = Graphics::CullMode::None,
                .drawType               = Graphics::DrawType::Triangle,
                .transparencyEnabled    = false,
                .depthBiasEnabled       = false,
                .depthTestEnabled       = false,
                .depthWriteEnabled      = false};
            // Create the pipeline
            m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

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
                },
                [=](const CompositeData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("Composition Pass");
                    RAZIX_MARK_BEGIN("Final Composition", glm::vec4(0.5f));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(compositionShader)->getBindVars();

                        RZDescriptor* descriptor = nullptr;

                        descriptor = shaderBindVars["CompositionTarget"];
                        if (descriptor)
                            descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(FinalOutputRenderTarget).getHandle();

                        RZResourceManager::Get().getShaderResource(compositionShader)->updateBindVarsHeaps();
                    }

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {
                        {Graphics::RHI::GetSwapchain()->GetCurrentImage(), {true, ClearColorPresets::OpaqueBlack}}};
                    info.resize = true;

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::ScreenQuad);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                },
                [=](FrameGraph::RZPassResourceDirectory& resources, u32 width, u32 height) {
                    RZGraphicsContext::GetContext()->Wait();
                });
#endif
        }

        void RZCompositionPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Graphics
}    // namespace Razix