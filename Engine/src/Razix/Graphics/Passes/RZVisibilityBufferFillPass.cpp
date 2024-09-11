// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZVisibilityBufferFillPass.h"

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

#include "Razix/Utilities/RZColorUtilities.h"

namespace Razix {
    namespace Graphics {

        void RZVisibilityBufferFillPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            auto shader = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::VisibilityBufferFill);

            RZPipelineDesc pipelineInfo         = {};
            pipelineInfo.name                   = "Pipeline.VisibilityBufferFill";
            pipelineInfo.shader                 = shader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::RGBA8};    // Pack into a unorm 8:8:8:8 texture
            pipelineInfo.depthFormat            = TextureFormat::DEPTH32F;
            pipelineInfo.cullMode               = Graphics::CullMode::Front;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameDataBlock = framegraph.getBlackboard().get<FrameData>();

            framegraph.getBlackboard().add<VisBufferData>() = framegraph.addCallbackPass<VisBufferData>(
                "Pass.Builtin.Code.VisibilityBufferFill",
                [&](VisBufferData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc visBufferTexturesDesc = {};
                    visBufferTexturesDesc.name          = "VisBuffer";
                    visBufferTexturesDesc.width         = RZApplication::Get().getWindow()->getWidth();
                    visBufferTexturesDesc.height        = RZApplication::Get().getWindow()->getHeight();
                    visBufferTexturesDesc.type          = TextureType::Texture_2D;
                    visBufferTexturesDesc.format        = TextureFormat::RGBA8;
                    data.visBuffer                      = builder.create<FrameGraph::RZFrameGraphTexture>(visBufferTexturesDesc.name, CAST_TO_FG_TEX_DESC visBufferTexturesDesc);

                    RZTextureDesc sceneDepthTexturesDesc = {};
                    sceneDepthTexturesDesc.name          = "SceneDepth";
                    sceneDepthTexturesDesc.width         = RZApplication::Get().getWindow()->getWidth();
                    sceneDepthTexturesDesc.height        = RZApplication::Get().getWindow()->getHeight();
                    sceneDepthTexturesDesc.filtering     = {Filtering::Mode::NEAREST, Filtering::Mode::NEAREST},
                    sceneDepthTexturesDesc.type          = TextureType::Texture_Depth;
                    sceneDepthTexturesDesc.format        = TextureFormat::DEPTH32F;
                    data.sceneDepth                      = builder.create<FrameGraph::RZFrameGraphTexture>(sceneDepthTexturesDesc.name, CAST_TO_FG_TEX_DESC sceneDepthTexturesDesc);

                    // This resource is created and written by this pass
                    data.visBuffer  = builder.write(data.visBuffer);
                    data.sceneDepth = builder.write(data.sceneDepth);

                    builder.read(frameDataBlock.frameData);
                },
                [=](const VisBufferData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("Vis.Buffer Fill Pass");
                    RAZIX_MARK_BEGIN("Vis.Buffer Fill Pass", Utilities::GenerateHashedColor4(29u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RZTextureHandle rt      = resources.get<FrameGraph::RZFrameGraphTexture>(data.visBuffer).getHandle();
                    RZTextureHandle depthRT = resources.get<FrameGraph::RZFrameGraphTexture>(data.sceneDepth).getHandle();

                    // TODO: Render at fixed resolution
                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{rt, {true, ClearColorPresets::OpaqueBlack}}};
                    info.depthAttachment  = {depthRT, {true, ClearColorPresets::DepthZeroToOne}};
                    info.resize           = true;

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Update descriptors on first frame
                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();

                        RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    }

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::SceneGeometry);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                });
        }

        void RZVisibilityBufferFillPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Graphics
}    // namespace Razix