#include "RZHelloTextureTestPass.h"

namespace Razix {
    namespace Gfx {

        void RZHelloTextureTestPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            rz_gfx_shader_desc desc = {};
            desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            desc.rzsfFilePath       = "//TestsRoot/GfxTests/HelloWorldTests/Shaders/Razix/Shader.Test.HelloTextureTest.rzsf";
            m_Shader                = Gfx::RZResourceManager::Get().createShader("Shader.GfxTest.HelloTexture", desc);

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
            m_TestTextureHandle = Gfx::CreateTextureFromFile("//RazixContent/Textures/TestCheckerMap.png");
            // Create a texture view for the test texture
            rz_gfx_resource_view_desc textureViewDesc      = {};
            textureViewDesc.descriptorType                 = RZ_GFX_DESCRIPTOR_TYPE_TEXTURE;
            textureViewDesc.textureViewDesc.pTexture       = RZResourceManager::Get().getTextureResource(m_TestTextureHandle);
            textureViewDesc.textureViewDesc.baseMip        = 0;
            textureViewDesc.textureViewDesc.baseArrayLayer = 0;
            textureViewDesc.textureViewDesc.dimension      = 1;
            m_TestTextureViewHandle                        = RZResourceManager::Get().createResourceView("Resview.GfxTest.TestTextureFullView", textureViewDesc);

            struct HelloTexturePassData
            {
                RZFrameGraphResource Depth;
            };

            // Register the shader bind map for this shader
            // This is used to manage the lifetime of the descriptor tables for this shader and manage binding resources to the shader
            RZResourceManager::Get().getShaderBindMap(m_Shader).RegisterBindMap(m_Shader);

            framegraph.addCallbackPass<HelloTexturePassData>(
                "[Test] Pass.Builtin.Code.HelloTexture",
                [&](HelloTexturePassData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    // TODO: Enable this for apple
                    // rz_gfx_texture_desc depthTextureDesc = {};
                    //depthTextureDesc.width               = RZApplication::Get().getWindow()->getWidth();
                    //depthTextureDesc.height              = RZApplication::Get().getWindow()->getHeight();
                    //depthTextureDesc.format              = TextureFormat::DEPTH16_UNORM;
                    //depthTextureDesc.type                = TextureType::kDepth;
                    //depthTextureDesc.resourceHints       = RZ_GFX_RESOURCE_VIEW_FLAG_DSV;
                    //data.Depth                           = builder.create<RZFrameGraphTexture>("SceneDepth", CAST_TO_FG_TEX_DESC depthTextureDesc);
                    //data.Depth = builder.write(data.Depth);

                    RZResourceManager::Get()
                        .getShaderBindMap(m_Shader)
                        .setDescriptorTable(RZEngine::Get().getWorldRenderer().getGlobalSamplerTable())
                        .setDescriptorBlacklist("Samplers", {"g_Sampler"})
                        .setResourceView("g_TestTexture", m_TestTextureViewHandle)
                        .validate()
                        .build();
                },
                [=](const HelloTexturePassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_TIME_STAMP_BEGIN("[Test] Hello Texture Pass");

                    rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
                    RAZIX_MARK_BEGIN(cmdBuffer, "[Test] Pass.Builtin.Code.HelloTexture", Utilities::GenerateHashedColor4(22u));

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
                    rz_gfx_descriptor_heap_handle heaps[] = {
                        RZEngine::Get().getWorldRenderer().getSamplerHeap(),
                        RZEngine::Get().getWorldRenderer().getResourceHeap(),
                    };

                    rzRHI_BindDescriptorHeaps(cmdBuffer, heaps, 2);

                    RZResourceManager::Get()
                        .getShaderBindMap(m_Shader)
                        .bind(cmdBuffer, RZ_GFX_PIPELINE_TYPE_GRAPHICS);

#define NUM_TRIANGLE_VERTS 3
                    rzRHI_DrawAuto(cmdBuffer, NUM_TRIANGLE_VERTS, 1, 0, 0);

                    rzRHI_EndRenderPass(cmdBuffer);

                    RAZIX_MARK_END(cmdBuffer);
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZHelloTextureTestPass::destroy()
        {
            RZResourceManager::Get()
                .getShaderBindMap(m_Shader)
                .destroy();    // This will destroy all the descriptor tables created/owned by this bind map
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            RZResourceManager::Get().destroyShader(m_Shader);
            RZResourceManager::Get().destroyResourceView(m_TestTextureViewHandle);
            RZResourceManager::Get().destroyTexture(m_TestTextureHandle);
        }
    }    // namespace Gfx
}    // namespace Razix
