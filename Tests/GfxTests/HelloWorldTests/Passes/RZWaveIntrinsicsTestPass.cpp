#include "RZWaveIntrinsicsTestPass.h"

namespace Razix {
    namespace Gfx {

        void RZWaveIntrinsicsTestPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            if (!g_GraphicsFeatures.support.SupportsWaveIntrinsics) {
                RAZIX_CORE_ERROR("[FrameGraph] Wave Intrinsics not supported on this GPU!");
                return;
            }

            // Create the shader and the pipeline
            rz_gfx_shader_desc desc = {};
            desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            desc.rzsfFilePath       = "//TestsRoot/GfxTests/HelloWorldTests/Shaders/Razix/Shader.Test.WaveIntrinsicsTest.rzsf";
            m_Shader                = RZResourceManager::Get().createShader("Shader.GfxTest.WaveIntrinsicsTest", desc);

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
            m_Pipeline                          = RZResourceManager::Get().createPipeline("Pipeline.GfxTest.WaveIntrinsics", pipelineInfo);

            struct WaveIntrinsicsData
            {
                RZFrameGraphResource Depth;
                RZFrameGraphResource DebugBuffer;
            };

            struct WaveIntrinsicsConstantBufferData
            {
                u32 waveMode = 1;
            };

            framegraph.getBlackboard()
                .add<WaveIntrinsicsData>() = framegraph.addCallbackPass<WaveIntrinsicsData>(
                "[Test] Pass.Builtin.Code.WaveIntrinsics",
                [&](WaveIntrinsicsData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    //RZTextureDesc depthTextureDesc;
                    //depthTextureDesc.name                  = "SceneDepth";
                    //depthTextureDesc.width                 = RZApplication::Get().getWindow()->getWidth();
                    //depthTextureDesc.height                = RZApplication::Get().getWindow()->getHeight();
                    //depthTextureDesc.format                = TextureFormat::DEPTH16_UNORM;
                    //depthTextureDesc.type                  = TextureType::kDepth;
                    //depthTextureDesc.initResourceViewHints = kDSV;
                    //data.Depth                             = builder.create<RZFrameGraphTexture>(depthTextureDesc.name, CAST_TO_FG_TEX_DESC depthTextureDesc);
                    //data.Depth                             = builder.write(data.Depth);

                    //RZBufferDesc DebugDataBufferDesc{};
                    //DebugDataBufferDesc.name  = "WaveIntrinsicsConstantBufferData";
                    //DebugDataBufferDesc.size  = sizeof(SSAOParamsData);
                    //DebugDataBufferDesc.usage = BufferUsage::PersistentStream;
                    //data.DebugBuffer          = builder.create<RZFrameGraphBuffer>(DebugDataBufferDesc.name, CAST_TO_FG_BUF_DESC DebugDataBufferDesc);
                    //
                    //data.DebugBuffer = builder.write(data.DebugBuffer);
                },
                [=](const WaveIntrinsicsData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_TIME_STAMP_BEGIN("[Test] WaveIntrinsics Pass");

                    rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
                    RAZIX_MARK_BEGIN(cmdBuffer, "[Test] Pass.Builtin.Code.WaveIntrinsics", Utilities::GenerateHashedColor4(45u));

                    rz_gfx_renderpass info                 = {0};
                    info.resolution                        = RZ_GFX_RESOLUTION_WINDOW;
                    info.colorAttachmentsCount             = 1;
                    info.colorAttachments[0].pResourceView = RZEngine::Get().getWorldRenderer().getCurrSwapchainBackbufferResViewPtr();
                    info.colorAttachments[0].clear         = true;
                    info.layers                            = 1;
                    RAZIX_X(info.extents)                  = RZApplication::Get().getWindow()->getWidth();
                    RAZIX_Y(info.extents)                  = RZApplication::Get().getWindow()->getHeight();

                    rzRHI_BeginRenderPass(cmdBuffer, &info);

                    rzRHI_BindGfxRootSig(cmdBuffer, m_RootSigHandle);
                    rzRHI_BindPipeline(cmdBuffer, m_Pipeline);

#define NUM_TRIANGLE_VERTS 3
                    rzRHI_DrawAuto(cmdBuffer, NUM_TRIANGLE_VERTS, 1, 0, 0);

                    rzRHI_EndRenderPass(cmdBuffer);

                    RAZIX_MARK_END(cmdBuffer);
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZWaveIntrinsicsTestPass::destroy()
        {
            if (g_GraphicsFeatures.support.SupportsWaveIntrinsics) {
                RZResourceManager::Get().destroyShader(m_Shader);
                RZResourceManager::Get().destroyPipeline(m_Pipeline);
            }
        }
    }    // namespace Gfx
}    // namespace Razix
