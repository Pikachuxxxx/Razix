// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGBufferPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
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

#include "Razix/Graphics/Passes/Data/FrameBlockData.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"

#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZGBufferPass::addPass(FrameGraph::RZFrameGraph& framegraph,  Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // First order of business get the shader
            auto shader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GBuffer);

            // Create the Pipeline
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                = "GBuffer Pipeline";
            pipelineInfo.cullMode            = Graphics::CullMode::None;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::Triangle;
            pipelineInfo.transparencyEnabled = true;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.depthTestEnabled    = true;
            // Using 32 bit f32ing point formats to support HDR colors
            pipelineInfo.colorAttachmentFormats = {
                Graphics::TextureFormat::RGBA32F,
                Graphics::TextureFormat::RGBA32F,
                Graphics::TextureFormat::RGBA32F};
            pipelineInfo.depthFormat = Graphics::TextureFormat::DEPTH32F;

            m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameDataBlock = framegraph.getBlackboard().get<FrameData>();

            framegraph.getBlackboard().add<GBufferData>() = framegraph.addCallbackPass<GBufferData>(
                "GBuffer",
                [&](GBufferData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc gbufferTexturesDesc{
                        .name   = "Normal_Metallic",
                        .width  = RZApplication::Get().getWindow()->getWidth(),
                        .height = RZApplication::Get().getWindow()->getHeight(),
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::RGBA32F};

                    data.Normal_Metallic = builder.create<FrameGraph::RZFrameGraphTexture>("Normal_Metallic", CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    gbufferTexturesDesc.name = "Albedo_Roughness";

                    data.Albedo_Roughness = builder.create<FrameGraph::RZFrameGraphTexture>("Albedo_Roughness", CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    gbufferTexturesDesc.name = "Position_AO";

                    data.Position_AO = builder.create<FrameGraph::RZFrameGraphTexture>("Position_AO", CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    gbufferTexturesDesc.name      = "Depth";
                    gbufferTexturesDesc.format    = TextureFormat::DEPTH32F;
                    gbufferTexturesDesc.filtering = {Filtering::Mode::NEAREST, Filtering::Mode::NEAREST},
                    gbufferTexturesDesc.type      = TextureType::Texture_Depth;

                    data.Depth = builder.create<FrameGraph::RZFrameGraphTexture>("Depth", CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    data.Normal_Metallic  = builder.write(data.Normal_Metallic);
                    data.Albedo_Roughness = builder.write(data.Albedo_Roughness);
                    data.Position_AO      = builder.write(data.Position_AO);
                    data.Depth            = builder.write(data.Depth);

                    builder.read(frameDataBlock.frameData);
                },
                [=](const GBufferData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_MARK_BEGIN("GBuffer Pass", glm::vec4(1.0f, 0.6f, 0.0f, 1.0f));

                    RenderingInfo info{
                        .extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()},
                        .colorAttachments = {
                            {resources.get<FrameGraph::RZFrameGraphTexture>(data.Normal_Metallic).getHandle(), {true, ClearColorPresets::TransparentBlack}},     // location = 0
                            {resources.get<FrameGraph::RZFrameGraphTexture>(data.Albedo_Roughness).getHandle(), {true, ClearColorPresets::TransparentBlack}},    // location = 1
                            {resources.get<FrameGraph::RZFrameGraphTexture>(data.Position_AO).getHandle(), {true, ClearColorPresets::TransparentBlack}},         // location = 2
                        },
                        .depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.Depth).getHandle(), {true}},
                        .resize          = false};

                    RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                    RHI::BindPipeline(m_Pipeline, RHI::GetCurrentCommandBuffer());

                    // Use scene to draw geometry
                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::SceneGeometry);

                    RAZIX_MARK_END();
                    RHI::EndRendering(RHI::GetCurrentCommandBuffer());
                });
        }

        void RZGBufferPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Graphics
}    // namespace Razix
