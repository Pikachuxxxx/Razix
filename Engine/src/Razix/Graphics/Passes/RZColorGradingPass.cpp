// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZColorGradingPass.h"

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

        void RZColorGradingPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // Create the shader and the pipeline
            auto shader = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::ColorGrading);

            RZPipelineDesc pipelineInfo{
                // Build the pipeline here for this pass
                .name                   = "ColorGrading.Pipeline",
                .shader                 = shader,
                .colorAttachmentFormats = {TextureFormat::RGBA32F},
                .cullMode               = Graphics::CullMode::None,
                .drawType               = Graphics::DrawType::Triangle,
                .transparencyEnabled    = false,
                .depthBiasEnabled       = false};

            m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& colorGradingLUTData = framegraph.getBlackboard().get<ColorGradingLUTData>();
            auto& sceneData           = framegraph.getBlackboard().get<SceneData>();

            framegraph.getBlackboard().add<ColorGradingData>() = framegraph.addCallbackPass<ColorGradingData>(
                "Pass.Builtin.Code.LUTColorGrading",
                [&](ColorGradingData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc colorGradedImageDesc{
                        .name   = "ColorGradedSceneHDR",
                        .width  = RZApplication::Get().getWindow()->getWidth(),
                        .height = RZApplication::Get().getWindow()->getHeight(),
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::RGBA32F};

                    data.colorGradedSceneHDR = builder.create<FrameGraph::RZFrameGraphTexture>(colorGradedImageDesc.name, CAST_TO_FG_TEX_DESC colorGradedImageDesc);

                    builder.read(sceneData.outputHDR);
                    builder.read(sceneData.depth);
                    builder.read(colorGradingLUTData.neutralLUT);

                    data.colorGradedSceneHDR = builder.write(data.colorGradedSceneHDR);
                },
                [=](const ColorGradingData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_MARK_BEGIN("Pass.Builtin.Code.LUTColorGrading", Utilities::GenerateHashedColor4(29u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(data.colorGradedSceneHDR).getHandle();

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

                        auto scene_descriptor = shaderBindVars["SceneHDRSource"];
                        if (scene_descriptor)
                            scene_descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.outputHDR).getHandle();

                        auto lut_descriptor = shaderBindVars["NeutralLUT"];
                        if (lut_descriptor)
                            lut_descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(colorGradingLUTData.neutralLUT).getHandle();

                        RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    }

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::ScreenQuad);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                });

            framegraph.getBlackboard().setFinalOutputName("ColorGradedSceneHDR");
        }

        void RZColorGradingPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Graphics
}    // namespace Razix