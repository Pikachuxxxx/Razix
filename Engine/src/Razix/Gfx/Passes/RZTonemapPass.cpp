// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZTonemapPass.h"

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

        void RZToneMapPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            auto tonemapShader = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Tonemap);

            RZPipelineDesc pipelineInfo{};
            // Build the pipeline here for this pass
            pipelineInfo.name                   = "Pipeline.Tonemap";
            pipelineInfo.shader                 = tonemapShader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::RGBA16F};
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.depthWriteEnabled      = false;
            // Create the pipeline
            m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& sceneData = framegraph.getBlackboard().get<SceneData>();

            framegraph.addCallbackPass(
                "Pass.Builtin.Code.Tonemap",
                [&](auto& data, RZPassResourceBuilder& builder) {
                    builder.read(sceneData.sceneHDR);
                },
                [=](const auto& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RETURN_IF_BIT_NOT_SET(settings->renderFeatures, RendererFeature_Tonemap);

                    RAZIX_TIME_STAMP_BEGIN("Tonemap Pass");
                    RAZIX_MARK_BEGIN("Tonemap Pass", Razix::Utilities::GenerateHashedColor4(169u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    if (RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(tonemapShader)->getBindVars();

                        RZDescriptor* descriptor = nullptr;

                        descriptor = shaderBindVars["SceneHDRRenderTarget"];
                        if (descriptor)
                            descriptor->texture = resources.get<RZFrameGraphTexture>(sceneData.sceneHDR).getHandle();

                        RZResourceManager::Get().getShaderResource(tonemapShader)->updateBindVarsHeaps();
                    }

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {
                        {resources.get<RZFrameGraphTexture>(sceneData.sceneHDR).getHandle(), {false, ClearColorPresets::OpaqueBlack}}};

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Push constant data for sending in the tone map mode
                    RZPushConstant pc = {};
                    pc.size           = sizeof(u32);
                    pc.data           = &(settings->tonemapMode);
                    pc.shaderStage    = ShaderStage::kPixel;
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
    }    // namespace Gfx
}    // namespace Razix
