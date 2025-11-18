#include "RZWaveIntrinsicsTestPass.h"

namespace Razix {
    namespace Gfx {

        void RZWaveIntrinsicsTestPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            if (!g_GraphicsFeatures.support.WaveIntrinsics) {
                RAZIX_CORE_ERROR("[FrameGraph] Wave Intrinsics not supported on this GPU!");
                return;
            }

            // Create the shader and the pipeline
            rz_gfx_shader_desc desc = {};
            desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            desc.rzsfFilePath       = "//TestsRoot/GfxTests/Shaders/Razix/Shader.Test.WaveIntrinsicsTest.rzsf";
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

            RZResourceManager::Get().getShaderBindMapRef(m_Shader).RegisterBindMap(m_Shader);

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

                    rz_gfx_buffer_desc DebugDataBufferDesc = {};
                    DebugDataBufferDesc.sizeInBytes        = sizeof(WaveIntrinsicsData);
                    DebugDataBufferDesc.type               = RZ_GFX_BUFFER_TYPE_CONSTANT;
                    DebugDataBufferDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_DYNAMIC;
                    DebugDataBufferDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_CBV;
                    data.DebugBuffer                       = builder.create<RZFrameGraphBuffer>("CBuffer.GfxText.WaveIntrinsicsData", CAST_TO_FG_BUF_DESC DebugDataBufferDesc);

                    rz_gfx_resource_view_desc debugBufferViewDesc = {};
                    debugBufferViewDesc.descriptorType            = RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER;
                    debugBufferViewDesc.bufferViewDesc.size       = sizeof(WaveIntrinsicsData);
                    debugBufferViewDesc.bufferViewDesc.offset     = 0;
                    debugBufferViewDesc.bufferViewDesc.pBuffer    = RZ_FG_BUF_RES_AUTO_POPULATE;
                    data.DebugBuffer                              = builder.write(data.DebugBuffer, debugBufferViewDesc);
                },
                [=](const WaveIntrinsicsData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_TIME_STAMP_BEGIN("[Test] WaveIntrinsics Pass");

                    rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
                    RAZIX_MARK_BEGIN(cmdBuffer, "[Test] Pass.Builtin.Code.WaveIntrinsics", GenerateHashedColor4(45u));

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

                    // Bind descriptor heaps and tables
                    const rz_gfx_descriptor_heap_handle heaps[] = {
                        RZEngine::Get().getWorldRenderer().getResourceHeap(),
                    };

                    rzRHI_BindDescriptorHeaps(cmdBuffer, heaps, 1);

                    if (RZFrameGraph::IsFirstFrame()) {
                        // Deferred creation of the shader bind map, for res view
                        RZResourceManager::Get()
                            .getShaderBindMapRef(m_Shader)
                            .setResourceView("g_WaveIntrinsicsConstantBuffer", resources.getResourceViewHandle<RZFrameGraphBuffer>(data.DebugBuffer))
                            .validate()
                            .build();
                    }

                    // Update the constant buffer data, we use wave mode of 3 (WaveGetLaneCount test)
                    rz_gfx_buffer_update cbUpdate           = {};
                    cbUpdate.pBuffer                        = RZResourceManager::Get().getBufferResource(resources.get<RZFrameGraphBuffer>(data.DebugBuffer).getRHIHandle());
                    cbUpdate.sizeInBytes                    = sizeof(WaveIntrinsicsConstantBufferData);
                    cbUpdate.offset                         = 0;
                    WaveIntrinsicsConstantBufferData cbData = {};
                    cbData.waveMode                         = 3;
                    cbUpdate.pData                          = &cbData;
                    rzRHI_UpdateMappedBuffer(cbUpdate);

                    RZResourceManager::Get()
                        .getShaderBindMapRef(m_Shader)
                        .bind(cmdBuffer, RZ_GFX_PIPELINE_TYPE_GRAPHICS);

#define NUM_TRIANGLE_VERTS 3
                    rzRHI_DrawAuto(cmdBuffer, NUM_TRIANGLE_VERTS, 1, 0, 0);

                    rzRHI_EndRenderPass(cmdBuffer);

                    RAZIX_MARK_END(cmdBuffer);
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZWaveIntrinsicsTestPass::destroy()
        {
            if (g_GraphicsFeatures.support.WaveIntrinsics) {
                RZResourceManager::Get()
                    .getShaderBindMapRef(m_Shader)
                    .destroy();
                RZResourceManager::Get().destroyShader(m_Shader);
                RZResourceManager::Get().destroyPipeline(m_Pipeline);
            }
        }
    }    // namespace Gfx
}    // namespace Razix
