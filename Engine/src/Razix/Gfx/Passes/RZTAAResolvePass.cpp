// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZTAAResolvePass.h"

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

        void RZTAAResolvePass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
#if 0
            // Create the shader and the pipeline
            auto shader = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::TAAResolve);

            RZPipelineDesc pipelineInfo{
                // Build the pipeline here for this pass
                .name                   = "TAAResolve.Pipeline",
                .shader                 = shader,
                .colorAttachmentFormats = {TextureFormat::RGBA16F},
                .cullMode               = Graphics::CullMode::None,
                .drawType               = Graphics::DrawType::Triangle,
                .transparencyEnabled    = false,
                .depthBiasEnabled       = false};

            auto& sceneData = framegraph.getBlackboard().get<SceneData>();

            m_Pipeline                                           = RZResourceManager::Get().createPipeline(pipelineInfo);
            framegraph.getBlackboard().add<FX::TAAResolveData>() = framegraph.addCallbackPass<FX::TAAResolveData>(
                "Pass.Builtin.Code.TAAResolve",
                [&](FX::TAAResolveData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    // Accumulation texture after TAA Resolve
                    RZTextureDesc accumalationTexDesc{
                        .name   = "RT.Scene.Accumulation",
                        .width  = RZApplication::Get().getWindow()->getWidth(),
                        .height = RZApplication::Get().getWindow()->getHeight(),
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::RGBA16F};

                    // Set this as the final composition target to apply tone mapping onto
                    sceneData.accumalationTexture = builder.create<RZFrameGraphTexture>(accumalationTexDesc.name, CAST_TO_FG_TEX_DESC accumalationTexDesc);
                    framegraph.getBlackboard().setFinalOutputName(accumalationTexDesc.name);

                    sceneData.accumalationTexture = builder.write(sceneData.accumalationTexture);

                    RZTextureDesc historyTexDesc{
                        .name   = "RT.Scene.History",
                        .width  = RZApplication::Get().getWindow()->getWidth(),
                        .height = RZApplication::Get().getWindow()->getHeight(),
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::RGBA16F};

                    sceneData.historyTexture = builder.create<RZFrameGraphTexture>(historyTexDesc.name, CAST_TO_FG_TEX_DESC historyTexDesc);

                    sceneData.historyTexture = builder.write(sceneData.historyTexture);

                    builder.read(sceneData.sceneHDR);
                    builder.read(sceneData.sceneDepth);

                    data = sceneData;
                },
                [=](const FX::TAAResolveData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_TIME_STAMP_BEGIN("TAA Resolve");
                    RAZIX_MARK_BEGIN("TAA Resolve", Utilities::GenerateHashedColor4(96u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    auto accumulationRT = resources.get<RZFrameGraphTexture>(data.accumalationTexture).getHandle();

                    RenderingInfo info{
                        .resolution       = Resolution::kWindow,
                        .colorAttachments = {{accumulationRT, {false, ClearColorPresets::OpaqueBlack}}},
                        .resize           = true};

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Update descriptors on first frame
                    if (RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();

                        // Bind the current Scene Texture and History Texture from last frame
                        auto currentTexDescriptor = shaderBindVars["CurrentTexture"];
                        if (currentTexDescriptor)
                            currentTexDescriptor->texture = resources.get<RZFrameGraphTexture>(sceneData.sceneHDR).getHandle();

                        auto historyTexDescriptor = shaderBindVars["HistoryTexture"];
                        if (historyTexDescriptor)
                            historyTexDescriptor->texture = resources.get<RZFrameGraphTexture>(data.historyTexture).getHandle();

                        RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    }

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::ScreenQuad);

                    RHI::EndRendering(cmdBuffer);

                    // Since we used the scene texture now copy it to the History Scene texture
                    RHI::CopyTextureResource(RHI::GetCurrentCommandBuffer(), resources.get<RZFrameGraphTexture>(data.historyTexture).getHandle(), resources.get<RZFrameGraphTexture>(sceneData.sceneHDR).getHandle());

                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
#endif
        }

        void RZTAAResolvePass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Gfx
}    // namespace Razix