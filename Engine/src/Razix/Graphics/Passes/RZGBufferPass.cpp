// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGBufferPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Passes/Data/FrameData.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"

#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

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
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                = "Pipeline.GBuffer ";
            pipelineInfo.cullMode            = Graphics::CullMode::Front;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::Triangle;
            pipelineInfo.transparencyEnabled = false;    // Deferred Shading historically doesn't support transparency so we disable it
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.depthTestEnabled    = true;
            // Using 32 bit f32ing point formats to support HDR colors
            pipelineInfo.colorAttachmentFormats = {
                Graphics::TextureFormat::RGBA16F,
                Graphics::TextureFormat::RGBA16F,
                Graphics::TextureFormat::RGBA16F,
                Graphics::TextureFormat::RG16F,
            };
            pipelineInfo.depthFormat = Graphics::TextureFormat::DEPTH32F;

            m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            pipelineInfo.polygonMode = PolygonMode::Line;
            m_WireframePipeline      = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameDataBlock = framegraph.getBlackboard().get<FrameData>();

            framegraph.getBlackboard().add<GBufferData>() = framegraph.addCallbackPass<GBufferData>(
                "Pass.Builtin.Code.GBuffer",
                [&](GBufferData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc gbufferTexturesDesc{
                        .name   = "gBuffer0",
                        .width  = RZApplication::Get().getWindow()->getWidth(),
                        .height = RZApplication::Get().getWindow()->getHeight(),
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::RGBA16F};

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
                    gbufferTexturesDesc.filtering = {Filtering::Mode::NEAREST, Filtering::Mode::NEAREST},
                    gbufferTexturesDesc.type      = TextureType::Texture_Depth;
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

                    RenderingInfo info{
                        .extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()},
                        .colorAttachments = {
                            {resources.get<FrameGraph::RZFrameGraphTexture>(data.GBuffer0).getHandle(), {true, ClearColorPresets::TransparentBlack}},
                            {resources.get<FrameGraph::RZFrameGraphTexture>(data.GBuffer1).getHandle(), {true, ClearColorPresets::TransparentBlack}},
                            {resources.get<FrameGraph::RZFrameGraphTexture>(data.GBuffer2).getHandle(), {true, ClearColorPresets::TransparentBlack}},
                            {resources.get<FrameGraph::RZFrameGraphTexture>(data.VelocityBuffer).getHandle(), {true, ClearColorPresets::TransparentBlack}},
                        },
                        .depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.GBufferDepth).getHandle(), {true, ClearColorPresets::DepthOneToZero}},
                        .resize          = true};

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
    }    // namespace Graphics
}    // namespace Razix
