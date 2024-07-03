// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZToneMapPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
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

        void RZToneMapPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            auto tonemapShader = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Tonemap);

            RZPipelineDesc pipelineInfo{};
            // Build the pipeline here for this pass
            pipelineInfo.name                   = "Pipeline.Tonemap";
            pipelineInfo.shader                 = tonemapShader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::RGBA16F};
            pipelineInfo.cullMode               = Graphics::CullMode::None;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.depthWriteEnabled      = false;
            // Create the pipeline
            m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& sceneData = framegraph.getBlackboard().get<SceneData>();

            framegraph.addCallbackPass(
                "Pass.Builtin.Code.Tonemap",
                [&](auto& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.read(sceneData.sceneHDR);

                    sceneData.sceneHDR = builder.write(sceneData.sceneHDR);
                },
                [=](const auto& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RETURN_IF_BIT_NOT_SET(settings->renderFeatures, RendererFeature_Tonemap);

                    RAZIX_TIME_STAMP_BEGIN("Tonemap Pass");
                    RAZIX_MARK_BEGIN("Tonemap Pass", Razix::Utilities::GenerateHashedColor4(169u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(tonemapShader)->getBindVars();

                        RZDescriptor* descriptor = nullptr;

                        descriptor = shaderBindVars["SceneHDRRenderTarget"];
                        if (descriptor)
                            descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneHDR).getHandle();

                        RZResourceManager::Get().getShaderResource(tonemapShader)->updateBindVarsHeaps();
                    }

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {
                        {resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneHDR).getHandle(), {false, ClearColorPresets::OpaqueBlack}}};
                    info.resize = true;

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Push constant data for sending in the tone map mode
                    RZPushConstant pc;
                    pc.size        = sizeof(u32);
                    pc.data        = &(settings->tonemapMode);
                    pc.shaderStage = ShaderStage::Pixel;
                    RHI::BindPushConstant(m_Pipeline, cmdBuffer, pc);

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::ScreenQuad);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZToneMapPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Graphics
}    // namespace Razix