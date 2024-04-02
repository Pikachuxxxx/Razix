// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZTAAResolvePass.h"

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

        void RZTAAResolvePass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // Create the shader and the pipeline
            auto shader = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::TAAResolve);

            RZPipelineDesc pipelineInfo{
                // Build the pipeline here for this pass
                .name                   = "TAAResolve.Pipeline",
                .shader                 = shader,
                .colorAttachmentFormats = {TextureFormat::RGBA32F},
                .cullMode               = Graphics::CullMode::None,
                .drawType               = Graphics::DrawType::Triangle,
                .transparencyEnabled    = false,
                .depthBiasEnabled       = false};

            m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);
            framegraph.addCallbackPass(
                "Pass.Builtin.Code.TAAResolve",
                [&](auto& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    // Accumulation texture after TAA Resolve
                },
                [=](const auto& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_MARK_BEGIN("TAA Resolve", Utilities::GenerateHashedColor4(96u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{
                        .resolution = Resolution::kWindow,
                        //.colorAttachments = {{rt, {true, ClearColorPresets::OpaqueBlack}}},
                        .resize = true};

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Update descriptors on first frame
                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();

                        // Bind the current Scene Texture and History Texture from last frame

                        RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    }

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::ScreenQuad);

                    // Since we used the scene texture not copy it to the History Scene texture

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                });
        }

        void RZTAAResolvePass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Graphics
}    // namespace Razix