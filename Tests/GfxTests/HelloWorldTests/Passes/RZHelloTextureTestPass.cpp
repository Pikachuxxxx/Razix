#include "RZHelloTextureTestPass.h"

namespace Razix {
    namespace Gfx {

        void RZHelloTextureTestPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            rz_gfx_shader_desc desc = {};
            desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            desc.rzsfFilePath       = "//TestsRoot/GfxTests/HelloWorldTests/Shaders/Razix/Shader.Test.HelloTextureTest.rzsf";
            m_Shader                = Gfx::RZResourceManager::Get().createShader("HelloTextureShader", desc);

            // Build the pipeline here for this pass
            rz_gfx_pipeline_desc pipelineInfo   = {};
            pipelineInfo.type                   = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            pipelineInfo.pShader                = RZResourceManager::Get().getShaderResource(m_Shader);
            m_RootSigHandle                     = pipelineInfo.pShader->rootSignature;
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

            // Import a test texture
            rz_gfx_texture_desc testTexDesc = {};
            testTexDesc.enableMips          = false;
            testTexDesc.filePath            = "//RazixContent/Textures/TestCheckerMap.png";
            m_TestTextureHandle             = RZResourceManager::Get().createTexture("TestCheckerTexture", testTexDesc);

            struct HelloTexturePassData
            {
                RZFrameGraphResource Depth;
            };

            framegraph.addCallbackPass<HelloTexturePassData>(
                "[Test] Pass.Builtin.Code.HelloTexture",
                [&](HelloTexturePassData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    rz_gfx_texture_desc depthTextureDesc   = {};
                    depthTextureDesc.name                  = "SceneDepth";
                    depthTextureDesc.width                 = RZApplication::Get().getWindow()->getWidth();
                    depthTextureDesc.height                = RZApplication::Get().getWindow()->getHeight();
                    depthTextureDesc.format                = TextureFormat::DEPTH16_UNORM;
                    depthTextureDesc.type                  = TextureType::kDepth;
                    depthTextureDesc.initResourceViewHints = kDSV;
                    data.Depth                             = builder.create<RZFrameGraphTexture>(depthTextureDesc.name, CAST_TO_FG_TEX_DESC depthTextureDesc);

                    data.Depth = builder.write(data.Depth);
                },
                [=](const HelloTexturePassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_TIME_STAMP_BEGIN("[Test] Hello Texture Pass");

                    rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
                    RAZIX_MARK_BEGIN(cmdBuffer, "[Test] Pass.Builtin.Code.HelloTexture", Utilities::GenerateHashedColor4(22u));


                    rz_gfx_renderpass info            = {0};
                    info.resolution                   = RZ_GFX_RESOLUTION_WINDOW;
                    info.colorAttachmentsCount        = 1;
                    info.colorAttachments[0].pTexture = RZEngine::Get().getWorldRenderer().getCurrSwapchainBackbufferPtr();
                    info.colorAttachments[0].clear    = true;
                    info.colorAttachments[0].mip      = 0;
                    info.colorAttachments[0].layer    = 0;
                    info.layers                       = 1;
                    RAZIX_X(info.extents)             = RZApplication::Get().getWindow()->getWidth();
                    RAZIX_Y(info.extents)             = RZApplication::Get().getWindow()->getHeight();

                    rzRHI_BeginRenderPass(cmdBuffer, &info);

                    rzRHI_BindGfxRootSig(cmdBuffer, m_RootSigHandle);
                    rzRHI_BindPipeline(cmdBuffer, m_Pipeline);

                // Bind descriptor heaps and tables (probably the bindless textures table?)

#define NUM_TRIANGLE_VERTS 3
                    rzRHI_DrawAuto(cmdBuffer, NUM_TRIANGLE_VERTS, 1, 0, 0);

                    rzRHI_EndRenderPass(cmdBuffer);

                    RAZIX_MARK_END(cmdBuffer);
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZHelloTextureTestPass::destroy()
        {
            RZResourceManager::Get().destroyTexture(m_TestTextureHandle);
            RZResourceManager::Get().destroyShader(m_Shader);
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Gfx
}    // namespace Razix
