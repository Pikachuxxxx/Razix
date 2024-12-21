// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGBufferPass.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"
#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZMeshFactory.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Materials/RZMaterial.h"

#include "Razix/Gfx/Passes/Data/FrameData.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"

#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Gfx {

        void RZGBufferPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // First order of business get the shader
            auto shader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GBuffer);

            /**
             * GBuffer RT formats
             * @slot #0 : XYZ = Normal          W = Metallic
             * @slot #1 : XYZ = Albedo          W = Roughness
             * @slot #2 : XYZ = World Position  W = AO (tex, not SSAO)
             * Scene Depth
             */

            // Create the Pipeline
            Gfx::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                = "Pipeline.GBuffer ";
            pipelineInfo.cullMode            = Gfx::CullMode::Front;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Gfx::DrawType::Triangle;
            pipelineInfo.transparencyEnabled = false;    // Deferred Shading historically doesn't support transparency so we disable it
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.depthTestEnabled    = true;
            // Using 32 bit f32ing point formats to support HDR colors
            pipelineInfo.colorAttachmentFormats = {
                Gfx::TextureFormat::RGBA16F,
                Gfx::TextureFormat::RGBA16F,
                Gfx::TextureFormat::RGBA16F,
                Gfx::TextureFormat::RG16F,
            };
            pipelineInfo.depthFormat = Gfx::TextureFormat::DEPTH32F;

            m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            pipelineInfo.polygonMode = PolygonMode::Line;
            m_WireframePipeline      = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameDataBlock = framegraph.getBlackboard().get<FrameData>();

            framegraph.getBlackboard().add<GBufferData>() = framegraph.addCallbackPass<GBufferData>(
                "Pass.Builtin.Code.GBuffer",
                [&](GBufferData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc gbufferTexturesDesc{};
                    gbufferTexturesDesc.name   = "gBuffer0";
                    gbufferTexturesDesc.width  = RZApplication::Get().getWindow()->getWidth();
                    gbufferTexturesDesc.height = RZApplication::Get().getWindow()->getHeight();
                    gbufferTexturesDesc.type   = TextureType::k2D;
                    gbufferTexturesDesc.format = TextureFormat::RGBA16F;

                    data.GBuffer0 = builder.create<FrameGraph::RZFrameGraphTexture>(gbufferTexturesDesc.name, CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    gbufferTexturesDesc.name = "gBuffer1";
                    data.GBuffer1            = builder.create<FrameGraph::RZFrameGraphTexture>(gbufferTexturesDesc.name, CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    gbufferTexturesDesc.name = "gBuffer2";
                    data.GBuffer2            = builder.create<FrameGraph::RZFrameGraphTexture>(gbufferTexturesDesc.name, CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    gbufferTexturesDesc.name   = "VelocityBuffer";
                    gbufferTexturesDesc.format = TextureFormat::RG16F;
                    data.VelocityBuffer        = builder.create<FrameGraph::RZFrameGraphTexture>(gbufferTexturesDesc.name, CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    gbufferTexturesDesc.name      = "SceneDepth";
                    gbufferTexturesDesc.format    = TextureFormat::DEPTH32F;
                    gbufferTexturesDesc.type      = TextureType::kDepth;
                    data.GBufferDepth             = builder.create<FrameGraph::RZFrameGraphTexture>(gbufferTexturesDesc.name, CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    data.GBuffer0       = builder.write(data.GBuffer0);
                    data.GBuffer1       = builder.write(data.GBuffer1);
                    data.GBuffer2       = builder.write(data.GBuffer2);
                    data.VelocityBuffer = builder.write(data.VelocityBuffer);
                    data.GBufferDepth   = builder.write(data.GBufferDepth);

                    builder.read(frameDataBlock.frameData);
                },
                [=](const GBufferData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("GBuffer Pass");
                    RAZIX_MARK_BEGIN("GBuffer Pass", glm::vec4(1.0f, 0.6f, 0.0f, 1.0f));

                    RenderingInfo info{};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.colorAttachments = {
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.GBuffer0).getHandle(), {true, ClearColorPresets::TransparentBlack}},
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.GBuffer1).getHandle(), {true, ClearColorPresets::TransparentBlack}},
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.GBuffer2).getHandle(), {true, ClearColorPresets::TransparentBlack}},
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.VelocityBuffer).getHandle(), {true, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.GBufferDepth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};
                    info.resize          = true;

                    RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                    RZPipelineHandle pipeline;
                    if (IS_BIT_SET(settings->debugFlags, RendererDebugFlag_VisWireframe))
                        pipeline = m_WireframePipeline;
                    else
                        pipeline = m_Pipeline;

                    RHI ::BindPipeline(pipeline, RHI::GetCurrentCommandBuffer());

                    // Use scene to draw geometry
                    scene->drawScene(pipeline, SceneDrawGeometryMode::SceneGeometry);

                    RHI::EndRendering(RHI::GetCurrentCommandBuffer());
                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZGBufferPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            RZResourceManager::Get().destroyPipeline(m_WireframePipeline);
        }
    }    // namespace Gfx
}    // namespace Razix
