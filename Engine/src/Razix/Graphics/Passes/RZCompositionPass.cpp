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

#include "Razix/Graphics/RHI/API/RZDrawCommandBuffer.h"
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

            RZPipelineDesc pipelineInfo{};
            // Build the pipeline here for this pass
            pipelineInfo.name                   = "Pipeline.Composition";
            pipelineInfo.shader                 = compositionShader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::BGRA8_UNORM};
            pipelineInfo.cullMode               = Graphics::CullMode::None;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.depthWriteEnabled      = false;
            // Create the pipeline
            m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            // Get the final output
            FrameGraph::RZFrameGraphResource FinalOutputRenderTarget = framegraph.getBlackboard().getFinalOutputID();
#if 1
            framegraph.addCallbackPass(
                "Pass.Builtin.Code.Composition",
                [&](auto& data, FrameGraph::RZPassResourceBuilder& builder) {
                    // Set this as a standalone pass (should not be culled)
                    builder.setAsStandAlonePass();

                    // Read the Final RT from where ever it's given from
                    builder.read(FinalOutputRenderTarget);
                },
                [=](const auto& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("Composition Pass");
                    RAZIX_MARK_BEGIN("Pass.Builtin.Code.Composition", glm::vec4(0.5f));

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