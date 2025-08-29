#include "RZMandleBrotPass.h"

namespace Razix {
    namespace Gfx {

        void RZMandleBrotPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* /*scene*/, RZRendererSettings* /*settings*/)
        {
            // Create compute shader
            rz_gfx_shader_desc shaderDesc = {};
            shaderDesc.pipelineType       = RZ_GFX_PIPELINE_TYPE_COMPUTE;
            shaderDesc.rzsfFilePath       = "//TestsRoot/GfxTests/Shaders/Razix/Shader.Test.MandleBrot.rzsf";    // placeholder path
            m_Shader                      = RZResourceManager::Get().createShader("Shader.GfxTest.MandleBrot", shaderDesc);

            // Create compute pipeline
            rz_gfx_pipeline_desc pipelineDesc = {};
            pipelineDesc.type                 = RZ_GFX_PIPELINE_TYPE_COMPUTE;
            pipelineDesc.pShader              = RZResourceManager::Get().getShaderResource(m_Shader);
            m_RootSigHandle                   = pipelineDesc.pShader->rootSignature;
            pipelineDesc.pRootSig             = RZResourceManager::Get().getRootSignatureResource(pipelineDesc.pShader->rootSignature);
            m_Pipeline                        = RZResourceManager::Get().createPipeline("Pipeline.GfxTest.MandleBrot", pipelineDesc);

            // Register shader bind map early
            RZResourceManager::Get()
                .getShaderBindMap(m_Shader)
                .RegisterBindMap(m_Shader);

            framegraph.getBlackboard().add<MandleBrotPassData>() = framegraph.addCallbackPass<MandleBrotPassData>(
                "[Test] Pass.Builtin.Code.MandleBrotCompute",
                [&](MandleBrotPassData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    rz_gfx_texture_desc outDesc = {};
                    outDesc.width               = RZApplication::Get().getWindow()->getWidth();
                    outDesc.height              = RZApplication::Get().getWindow()->getHeight();
                    outDesc.mipLevels           = 1;
                    outDesc.depth               = 1;
                    outDesc.arraySize           = 1;
                    outDesc.format              = RZ_GFX_FORMAT_B8G8R8A8_UNORM;
                    outDesc.textureType         = RZ_GFX_TEXTURE_TYPE_2D;
                    outDesc.resourceHints       = (rz_gfx_resource_view_hints) (RZ_GFX_RESOURCE_VIEW_FLAG_UAV | RZ_GFX_RESOURCE_VIEW_FLAG_SRV);
                    data.Output                 = builder.create<RZFrameGraphTexture>("Texture.MandleBrot.Output", CAST_TO_FG_TEX_DESC outDesc);

                    // Write with UAV view
                    rz_gfx_resource_view_desc uavView      = {};
                    uavView.descriptorType                 = RZ_GFX_DESCRIPTOR_TYPE_RW_TEXTURE;
                    uavView.textureViewDesc.pTexture       = RZ_FG_TEX_RES_AUTO_POPULATE;
                    uavView.textureViewDesc.baseMip        = 0;
                    uavView.textureViewDesc.baseArrayLayer = 0;
                    data.Output                            = builder.write(data.Output, uavView);
                },
                [=](const MandleBrotPassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_TIME_STAMP_BEGIN("[Test] MandleBrot Compute Pass");

                    rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
                    RAZIX_MARK_BEGIN(cmdBuffer, "[Test] Pass.Builtin.Code.MandleBrotCompute", Utilities::GenerateHashedColor4(111u));

                    // Bind descriptor heaps and tables
                    rz_gfx_descriptor_heap_handle heaps[] = {
                        RZEngine::Get().getWorldRenderer().getResourceHeap(),
                    };

                    rzRHI_BindDescriptorHeaps(cmdBuffer, heaps, 1);

                    if (RZFrameGraph::IsFirstFrame()) {
                        RZResourceManager::Get()
                            .getShaderBindMap(m_Shader)
                            .setResourceView("g_OutputTexture", resources.getResourceViewHandle<RZFrameGraphTexture>(data.Output))
                            .validate()
                            .build();
                    }

                    // Bind compute root signature and pipeline
                    rzRHI_BindComputeRootSig(cmdBuffer, m_RootSigHandle);
                    rzRHI_BindPipeline(cmdBuffer, m_Pipeline);

                    RZResourceManager::Get().getShaderBindMap(m_Shader).bind(cmdBuffer, RZ_GFX_PIPELINE_TYPE_COMPUTE);

                    u32 width      = RZApplication::Get().getWindow()->getWidth();
                    u32 height     = RZApplication::Get().getWindow()->getHeight();
                    u32 groupSizeX = 16;
                    u32 groupSizeY = 16;
                    u32 dispatchX  = (width + groupSizeX - 1) / groupSizeX;
                    u32 dispatchY  = (height + groupSizeY - 1) / groupSizeY;
                    rzRHI_Dispatch(cmdBuffer, dispatchX, dispatchY, 1);

                    RAZIX_MARK_END(cmdBuffer);
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZMandleBrotPass::destroy()
        {
            RZResourceManager::Get().getShaderBindMap(m_Shader).destroy();
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            RZResourceManager::Get().destroyShader(m_Shader);
        }
    }    // namespace Gfx
}    // namespace Razix
