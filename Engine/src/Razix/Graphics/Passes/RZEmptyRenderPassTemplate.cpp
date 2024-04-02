// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZEmptyRenderPassTemplate.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Utilities/RZColorUtilities.h"

namespace Razix {
    namespace Graphics {

        void RZEmptyRenderPassTemplate::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // Create the shader and the pipeline
            //auto shader = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::);
            //
            //RZPipelineDesc pipelineInfo{
            //    // Build the pipeline here for this pass
            //    .name                   = "ColorGrading.Pipeline",
            //    .shader                 = shader,
            //    .colorAttachmentFormats = {TextureFormat::RGBA32F},
            //    .cullMode               = Graphics::CullMode::None,
            //    .drawType               = Graphics::DrawType::Triangle,
            //    .transparencyEnabled    = false,
            //    .depthBiasEnabled       = false};
            //
            //m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);
            framegraph.addCallbackPass(
                "",
                [&](auto& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();
                },
                [=](const auto& data, FrameGraph::RZPassResourceDirectory& resources) {
#if 0
                    
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_MARK_BEGIN("", Utilities::GenerateHashedColor4(45u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{
                        .resolution       = Resolution::kWindow,
                        .colorAttachments = {{rt, {true, ClearColorPresets::OpaqueBlack}}},
                        .resize           = true};

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Update descriptors on first frame
                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();

                        RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    }

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::ScreenQuad);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
#endif
                });

            framegraph.getBlackboard().setFinalOutputName("ColorGradedSceneHDR");
        }

        void RZEmptyRenderPassTemplate::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Graphics
}    // namespace Razix