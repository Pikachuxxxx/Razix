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

            struct HelloTriangleData
            {
                RZFrameGraphResource Depth;
            };

            framegraph.getBlackboard().add<HelloTriangleData>() = framegraph.addCallbackPass<HelloTriangleData>(
                "[Test] Pass.Builtin.Code.HelloTriangle",
                [&](HelloTriangleData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();
                },
                [=](const HelloTriangleData& data, RZPassResourceDirectory& resources) {
                    RAZIX_TIME_STAMP_BEGIN("[Test] Hello Triangle Pass");
                    RAZIX_MARK_BEGIN("[Test] Pass.Builtin.Code.HelloTriangle", Utilities::GenerateHashedColor4(69u));

                    rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();

                    rz_gfx_renderpass info            = {0};
                    info.resolution                   = RZ_GFX_RESOLUTION_WINDOW;
                    info.colorAttachmentsCount        = 1;
                    info.colorAttachments[0].pTexture = &RZEngine::Get().getWorldRenderer().getCurrSwapchainBackbuffer();
                    info.colorAttachments[0].clear    = true;
                    info.colorAttachments[0].mip      = 0;
                    info.colorAttachments[0].layer    = 0;
                    info.layers                       = 1;
                    RAZIX_X(info.extents)             = RZApplication::Get().getWindow()->getWidth();
                    RAZIX_Y(info.extents)             = RZApplication::Get().getWindow()->getHeight();

                    rzRHI_BeginRenderPass(cmdBuffer, info);

                    rzRHI_BindGfxRootSig(cmdBuffer, m_RootSigHandle);
                    rzRHI_BindPipeline(cmdBuffer, m_Pipeline);

#define NUM_TRIANGLE_VERTS 3
                    rzRHI_DrawAuto(cmdBuffer, NUM_TRIANGLE_VERTS, 1, 0, 0);

                    rzRHI_EndRenderPass(cmdBuffer);

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
