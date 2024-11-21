// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZHelloTrianglePass.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/RZEngine.h"
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

        void RZHelloTrianglePass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            auto shader = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::HelloTriangle);

            RZPipelineDesc pipelineInfo{};
            // Build the pipeline here for this pass
            pipelineInfo.name                   = "Pipeline.HelloTriangle";
            pipelineInfo.shader                 = shader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::SCREEN};
            pipelineInfo.depthFormat            = TextureFormat::DEPTH16_UNORM;
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.depthWriteEnabled      = false;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);
            
            struct HelloTriangleData
            {
                FrameGraph::RZFrameGraphResource Depth;
            };

            framegraph.getBlackboard().add<HelloTriangleData>() = framegraph.addCallbackPass<HelloTriangleData>(
                "Pass.Builtin.Code.HelloTriangle",
                [&](HelloTriangleData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();
                    
                    RZTextureDesc depthTextureDesc;
                    depthTextureDesc.name      = "SceneDepth";
                    depthTextureDesc.width     = RZApplication::Get().getWindow()->getWidth();
                    depthTextureDesc.height    = RZApplication::Get().getWindow()->getHeight();
                    depthTextureDesc.format    = TextureFormat::DEPTH16_UNORM;
                    depthTextureDesc.filtering = {Filtering::Mode::NEAREST, Filtering::Mode::NEAREST},
                    depthTextureDesc.type      = TextureType::Texture_Depth;
                    data.Depth                 = builder.create<FrameGraph::RZFrameGraphTexture>(depthTextureDesc.name, CAST_TO_FG_TEX_DESC depthTextureDesc);

                },
                [=](const HelloTriangleData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("Hello Triangle Pass");
                    RAZIX_MARK_BEGIN("Pass.Builtin.Code.HelloTriangle", Utilities::GenerateHashedColor4(69u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{Gfx::RHI::GetSwapchain()->GetCurrentImage(), {true, ClearColorPresets::OpaqueBlack}}};
                    
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.Depth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};
                    
                    info.resize           = true;

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Draw 3 vertices
                    Gfx::RHI::Draw(cmdBuffer, 3);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZHelloTrianglePass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Graphics
}    // namespace Razix
