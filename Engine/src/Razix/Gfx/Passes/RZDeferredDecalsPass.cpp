// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZDeferredDecalsPass.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"

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

        void RZDeferredDecalsPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            auto shader = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::ColorGrading);

            RZPipelineDesc pipelineInfo = {};
            // Build the pipeline here for this pass
            pipelineInfo.name                   = "ColorGrading.Pipeline";
            pipelineInfo.shader                 = shader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::RGBA16F};
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;

            m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& colorGradingLUTData = framegraph.getBlackboard().get<FX::ColorGradingLUTData>();
            auto& sceneData           = framegraph.getBlackboard().get<SceneData>();

            framegraph.getBlackboard().add<FX::ColorGradingData>() = framegraph.addCallbackPass<FX::ColorGradingData>(
                "Pass.Builtin.Code.LUTColorGrading",
                [&](FX::ColorGradingData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc colorGradedImageDesc{};
                    colorGradedImageDesc.name   = "ColorGradedSceneHDR";
                    colorGradedImageDesc.width  = RZApplication::Get().getWindow()->getWidth();
                    colorGradedImageDesc.height = RZApplication::Get().getWindow()->getHeight();
                    colorGradedImageDesc.type   = TextureType::k2D;
                    colorGradedImageDesc.format = TextureFormat::RGBA16F;

                    data.colorGradedSceneHDR = builder.create<FrameGraph::RZFrameGraphTexture>(colorGradedImageDesc.name, CAST_TO_FG_TEX_DESC colorGradedImageDesc);

                    builder.read(sceneData.sceneHDR);
                    builder.read(sceneData.sceneDepth);
                    builder.read(colorGradingLUTData.neutralLUT);

                    data.colorGradedSceneHDR = builder.write(data.colorGradedSceneHDR);
                },
                [=](const FX::ColorGradingData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_MARK_BEGIN("Pass.Builtin.Code.LUTColorGrading", Utilities::GenerateHashedColor4(29u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(data.colorGradedSceneHDR).getHandle();

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{rt, {true, ClearColorPresets::OpaqueBlack}}};

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Update descriptors on first frame
                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();

                        auto scene_descriptor = shaderBindVars["SceneHDRSource"];
                        if (scene_descriptor)
                            scene_descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneHDR).getHandle();

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

        void RZDeferredDecalsPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Gfx
}    // namespace Razix