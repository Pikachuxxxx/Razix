// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZVisibilityBufferFillPass.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZShader.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"
#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZMeshFactory.h"
#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Materials/RZMaterial.h"

#include "Razix/Gfx/Passes/Data/GlobalData.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZScene.h"

#include "Razix/Utilities/RZColorUtilities.h"

namespace Razix {
    namespace Gfx {

        void RZVisibilityBufferFillPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            auto shader = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::VisibilityBufferFill);

            RZPipelineDesc pipelineInfo         = {};
            pipelineInfo.name                   = "Pipeline.VisibilityBufferFill";
            pipelineInfo.shader                 = shader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::RGBA8};    // Pack into a unorm 8:8:8:8 texture
            pipelineInfo.depthFormat            = TextureFormat::DEPTH32F;
            pipelineInfo.cullMode               = Gfx::CullMode::Front;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameDataBlock = framegraph.getBlackboard().get<FrameData>();

            framegraph.getBlackboard().add<VisBufferData>() = framegraph.addCallbackPass<VisBufferData>(
                "Pass.Builtin.Code.VisibilityBufferFill",
                [&](VisBufferData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc visBufferTexturesDesc         = {};
                    visBufferTexturesDesc.name                  = "VisBuffer";
                    visBufferTexturesDesc.width                 = RZApplication::Get().getWindow()->getWidth();
                    visBufferTexturesDesc.height                = RZApplication::Get().getWindow()->getHeight();
                    visBufferTexturesDesc.type                  = TextureType::k2D;
                    visBufferTexturesDesc.format                = TextureFormat::RGBA8;
                    visBufferTexturesDesc.allowResize           = true;
                    visBufferTexturesDesc.initResourceViewHints = kSRV | kRTV;
                    data.visBuffer                              = builder.create<RZFrameGraphTexture>(visBufferTexturesDesc.name, CAST_TO_FG_TEX_DESC visBufferTexturesDesc);

                    RZTextureDesc sceneDepthTexturesDesc         = {};
                    sceneDepthTexturesDesc.name                  = "SceneDepth";
                    sceneDepthTexturesDesc.width                 = RZApplication::Get().getWindow()->getWidth();
                    sceneDepthTexturesDesc.height                = RZApplication::Get().getWindow()->getHeight();
                    sceneDepthTexturesDesc.type                  = TextureType::kDepth;
                    sceneDepthTexturesDesc.format                = TextureFormat::DEPTH32F;
                    sceneDepthTexturesDesc.allowResize           = true;
                    sceneDepthTexturesDesc.initResourceViewHints = kDSV;
                    data.sceneDepth                              = builder.create<RZFrameGraphTexture>(sceneDepthTexturesDesc.name, CAST_TO_FG_TEX_DESC sceneDepthTexturesDesc);

                    // This resource is created and written by this pass
                    data.visBuffer  = builder.write(data.visBuffer);
                    data.sceneDepth = builder.write(data.sceneDepth);

                    builder.read(frameDataBlock.frameData);
                },
                [=](const VisBufferData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("Vis.Buffer Fill Pass");
                    RAZIX_MARK_BEGIN("Vis.Buffer Fill Pass", Utilities::GenerateHashedColor4(29u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    rz_texture_handle rt      = resources.get<RZFrameGraphTexture>(data.visBuffer).getHandle();
                    rz_texture_handle depthRT = resources.get<RZFrameGraphTexture>(data.sceneDepth).getHandle();

                    // TODO: Render at fixed resolution
                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{rt, {true, ClearColorPresets::OpaqueBlack}}};
                    info.depthAttachment  = {depthRT, {true, ClearColorPresets::DepthZeroToOne}};

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Update descriptors on first frame
                    if (RZFrameGraph::IsFirstFrame()) {
                        RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    }

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::SceneGeometry);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZVisibilityBufferFillPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Gfx
}    // namespace Razix
