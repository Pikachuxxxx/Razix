#include "RZHelloTriangleTestPass.h"

namespace Razix {
    namespace Gfx {

        void RZHelloTriangleTestPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            RZShaderDesc desc = {};
            desc.filePath     = "//TestsRoot/GfxTests/HelloWorldTests/Shaders/Razix/Shader.Test.HelloTriangleTest.rzsf";
            desc.libraryID = ShaderBuiltin::Default;
            desc.name = "HelloTriangle";
            m_Shader          = RZResourceManager::Get().createShader(desc);

            RZPipelineDesc pipelineInfo{};
            // Build the pipeline here for this pass
            pipelineInfo.name                   = "[Test] Pipeline.HelloTriangle";
            pipelineInfo.shader                 = m_Shader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::SCREEN};
            pipelineInfo.depthFormat            = TextureFormat::DEPTH16_UNORM;
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            struct HelloTriangleData
            {
                FrameGraph::RZFrameGraphResource Depth;
            };

            framegraph.getBlackboard().add<HelloTriangleData>() = framegraph.addCallbackPass<HelloTriangleData>(
                "[Test] Pass.Builtin.Code.HelloTriangle",
                [&](HelloTriangleData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc depthTextureDesc;
                    depthTextureDesc.name                  = "SceneDepth";
                    depthTextureDesc.width                 = RZApplication::Get().getWindow()->getWidth();
                    depthTextureDesc.height                = RZApplication::Get().getWindow()->getHeight();
                    depthTextureDesc.format                = TextureFormat::DEPTH16_UNORM;
                    depthTextureDesc.type                  = TextureType::kDepth;
                    depthTextureDesc.initResourceViewHints = kDSV;
                    data.Depth                             = builder.create<FrameGraph::RZFrameGraphTexture>(depthTextureDesc.name, CAST_TO_FG_TEX_DESC depthTextureDesc);
                    data.Depth                             = builder.write(data.Depth);
                },
                [=](const HelloTriangleData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_TIME_STAMP_BEGIN("[Test] Hello Triangle Pass");
                    RAZIX_MARK_BEGIN("[Test] Pass.Builtin.Code.HelloTriangle", Utilities::GenerateHashedColor4(69u));

                    auto cmdBuffer = Gfx::RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{Gfx::RHI::GetSwapchain()->GetCurrentImage(), {true, ClearColorPresets::OpaqueBlack}}};
                    info.depthAttachment  = {resources.get<FrameGraph::RZFrameGraphTexture>(data.Depth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};
                    info.resize           = true;

                    Gfx::RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    Gfx::RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    constexpr u32 NumTriangleVerts = 3;
                    Gfx::RHI::Draw(cmdBuffer, NumTriangleVerts);

                    Gfx::RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZHelloTriangleTestPass::destroy()
        {
            RZResourceManager::Get().destroyShader(m_Shader);
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Gfx
}    // namespace Razix
