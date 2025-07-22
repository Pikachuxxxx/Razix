#include "RZHelloTriangleTestPass.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

namespace Razix {
    namespace Gfx {

        void RZHelloTriangleTestPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            rz_gfx_shader_desc desc = {};
            desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            desc.rzsfFilePath       = "//TestsRoot/GfxTests/HelloWorldTests/Shaders/Razix/Shader.Test.HelloTriangleTest.rzsf";
            m_Shader                = Gfx::RZResourceManager::Get().createShader("HelloTriangleShader", desc);

            // Build the pipeline here for this pass
            rz_gfx_pipeline_desc pipelineInfo   = {};
            pipelineInfo.type                   = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            pipelineInfo.pShader                = RZResourceManager::Get().getShaderResource(m_Shader);
            pipelineInfo.pRootSig               = RZResourceManager::Get().getRootSignatureResource(pipelineInfo.pShader->rootSignature);
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.depthWriteEnabled      = false;
            pipelineInfo.cullMode               = RZ_GFX_CULL_MODE_TYPE_NONE;
            pipelineInfo.drawType               = RZ_GFX_DRAW_TYPE_TRIANGLE;
            pipelineInfo.blendEnabled           = false;
            pipelineInfo.useBlendPreset         = true;
            pipelineInfo.blendPreset            = RZ_GFX_BLEND_PRESET_ADDITIVE;
            pipelineInfo.renderTargetCount      = 1;
            pipelineInfo.renderTargetFormats[0] = RZ_GFX_FORMAT_SCREEN;

            m_Pipeline = RZResourceManager::Get().createPipeline("Pipeline.GfxTest.HelloTriangle", pipelineInfo);

#if 0
            struct HelloTriangleData
            {
                RZFrameGraphResource Depth;
            };

            framegraph.getBlackboard().add<HelloTriangleData>() = framegraph.addCallbackPass<HelloTriangleData>(
                "[Test] Pass.Builtin.Code.HelloTriangle",
                [&](HelloTriangleData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

    #ifdef __APPLE__
                    RZTextureDesc depthTextureDesc;
                    depthTextureDesc.name                  = "SceneDepth";
                    depthTextureDesc.width                 = RZApplication::Get().getWindow()->getWidth();
                    depthTextureDesc.height                = RZApplication::Get().getWindow()->getHeight();
                    depthTextureDesc.format                = TextureFormat::DEPTH16_UNORM;
                    depthTextureDesc.type                  = TextureType::kDepth;
                    depthTextureDesc.initResourceViewHints = kDSV;
                    data.Depth                             = builder.create<RZFrameGraphTexture>(depthTextureDesc.name, CAST_TO_FG_TEX_DESC depthTextureDesc);
                    data.Depth                             = builder.write(data.Depth);
    #endif
                },
                [=](const HelloTriangleData& data, RZPassResourceDirectory& resources) {
                    RAZIX_TIME_STAMP_BEGIN("[Test] Hello Triangle Pass");
                    RAZIX_MARK_BEGIN("[Test] Pass.Builtin.Code.HelloTriangle", Utilities::GenerateHashedColor4(69u));

                    auto cmdBuffer = Gfx::RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{Gfx::RHI::GetSwapchain()->GetCurrentBackBufferImage(), {true, ClearColorPresets::OpaqueBlack}}};
    #ifdef __APPLE__
                    info.depthAttachment = {resources.get<RZFrameGraphTexture>(data.Depth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};
    #endif

                    Gfx::RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    Gfx::RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    constexpr u32 NumTriangleVerts = 3;
                    Gfx::RHI::Draw(cmdBuffer, NumTriangleVerts);

                    Gfx::RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
#endif
        }

        void RZHelloTriangleTestPass::destroy()
        {
            RZResourceManager::Get().destroyShader(m_Shader);
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Gfx
}    // namespace Razix
