// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZEmptyRenderPassTemplate.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZShader.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Utilities/RZColorUtilities.h"

namespace Razix {
    namespace Gfx {

        void RZEmptyRenderPassTemplate::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            //auto shader = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::);
            //
            //RZPipelineDesc pipelineInfo         = {};
            //pipelineInfo.name                   = "Pipeline.VisibilityBuffer";
            //pipelineInfo.shader                 = shader;
            //pipelineInfo.colorAttachmentFormats = {TextureFormat::RGBA16F};
            //pipelineInfo.cullMode               = Graphics::CullMode::None;
            //pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            //pipelineInfo.transparencyEnabled    = false;
            //pipelineInfo.depthBiasEnabled       = false;
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
    }    // namespace Gfx
}    // namespace Razix