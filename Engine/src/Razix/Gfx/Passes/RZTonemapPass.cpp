// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZTonemapPass.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Passes/GlobalData.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"
#include "Razix/Gfx/Resources/RZResourceManager.h"

#include "Razix/Core/Utils/RZColorUtilities.h"
#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Gfx {

        void RZToneMapPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            auto tonemapShader = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::kTonemap);

            rz_gfx_pipeline_desc pipelineDesc   = {};
            pipelineDesc.pShader                = RZResourceManager::Get().getShaderResource(tonemapShader);
            m_RootSignature                     = pipelineDesc.pShader->rootSignature;
            pipelineDesc.pRootSig               = RZResourceManager::Get().getRootSignatureResource(m_RootSignature);
            pipelineDesc.type                   = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            pipelineDesc.cullMode               = RZ_GFX_CULL_MODE_TYPE_NONE;
            pipelineDesc.polygonMode            = RZ_GFX_POLYGON_MODE_TYPE_SOLID;
            pipelineDesc.drawType               = RZ_GFX_DRAW_TYPE_TRIANGLE;
            pipelineDesc.depthTestEnabled       = false;
            pipelineDesc.depthWriteEnabled      = false;
            pipelineDesc.blendEnabled           = false;
            pipelineDesc.depthCompareOp         = RZ_GFX_COMPARE_OP_TYPE_LESS_OR_EQUAL;
            pipelineDesc.renderTargetCount      = 1;
            pipelineDesc.renderTargetFormats[0] = RZ_GFX_FORMAT_R8G8B8A8_UNORM;
            pipelineDesc.inputLayoutMode        = RZ_GFX_INPUT_LAYOUT_AOS;
            m_Pipeline                          = RZResourceManager::Get().createPipeline("Pipeline.Tonemapping", pipelineDesc);

            auto& sceneData = framegraph.getBlackboard().get<SceneData>();

            // since exec lambda captures by copy, we need to copy the scene data here and pass the copy to the lambda from framegraph backend via data
            struct ToneMapPassData
            {
                RZFrameGraphResource LDR;
            };

            RZShaderBindMap::RegisterBindMap(tonemapShader);

            framegraph.addCallbackPass<ToneMapPassData>(
                "Pass.Builtin.Code.Tonemap",
                [&](ToneMapPassData& data, RZPassResourceBuilder& builder) {
                    builder
                        .setAsStandAlonePass()
                        .setDepartment(Department::Lighting);

                    rz_gfx_resource_view_desc sceneHDRViewDesc      = {};
                    sceneHDRViewDesc.descriptorType                 = RZ_GFX_DESCRIPTOR_TYPE_TEXTURE;
                    sceneHDRViewDesc.textureViewDesc.pTexture       = RZ_FG_TEX_RES_AUTO_POPULATE;
                    sceneHDRViewDesc.textureViewDesc.baseMip        = 0;
                    sceneHDRViewDesc.textureViewDesc.baseArrayLayer = 0;
                    sceneHDRViewDesc.textureViewDesc.dimension      = 1;
                    sceneHDRViewDesc.pRtvDsvHeap                    = RZResourceManager::Get().getDescriptorHeapResource(RZEngine::Get().getWorldRenderer().getRenderTargetHeap());
                    builder.read(sceneData.HDR, sceneHDRViewDesc);

                    // Render onto LDR target
                    rz_gfx_texture_desc sceneLDRTextureDesc         = {};
                    sceneLDRTextureDesc.width                       = RZApplication::Get().getWindow()->getWidth();
                    sceneLDRTextureDesc.height                      = RZApplication::Get().getWindow()->getHeight();
                    sceneLDRTextureDesc.depth                       = 1;
                    sceneLDRTextureDesc.format                      = RZ_GFX_FORMAT_R8G8B8A8_UNORM;
                    sceneLDRTextureDesc.textureType                 = RZ_GFX_TEXTURE_TYPE_2D;
                    sceneLDRTextureDesc.mipLevels                   = 1;
                    sceneLDRTextureDesc.resourceHints               = rz_gfx_resource_view_hints(RZ_GFX_RESOURCE_VIEW_FLAG_RTV | RZ_GFX_RESOURCE_VIEW_FLAG_SRV);
                    data.LDR                                        = builder.create<RZFrameGraphTexture>("FG.Tex.SceneLDR", CAST_TO_FG_TEX_DESC sceneLDRTextureDesc);
                    rz_gfx_resource_view_desc sceneLDRViewDesc      = {};
                    sceneLDRViewDesc.descriptorType                 = RZ_GFX_DESCRIPTOR_TYPE_RENDER_TEXTURE;
                    sceneLDRViewDesc.textureViewDesc.pTexture       = RZ_FG_TEX_RES_AUTO_POPULATE;
                    sceneLDRViewDesc.textureViewDesc.baseMip        = 0;
                    sceneLDRViewDesc.textureViewDesc.baseArrayLayer = 0;
                    sceneLDRViewDesc.textureViewDesc.dimension      = 1;
                    sceneLDRViewDesc.pRtvDsvHeap                    = RZResourceManager::Get().getDescriptorHeapResource(RZEngine::Get().getWorldRenderer().getRenderTargetHeap());
                    data.LDR                                        = builder.write(data.LDR, sceneLDRViewDesc);

                    sceneData.LDR = data.LDR;
                },
                [=](const ToneMapPassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RETURN_IF_BIT_NOT_SET(settings->renderFeatures, RendererFeature_Tonemap);

                    RAZIX_TIME_STAMP_BEGIN("Tonemap Pass");
                    rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
                    RAZIX_MARK_BEGIN(cmdBuffer, "Tonemap Pass", GenerateHashedColor4(169u));

                    rzRHI_InsertImageBarrier(cmdBuffer, resources.get<RZFrameGraphTexture>(data.LDR).getRHIHandle(), RZ_GFX_RESOURCE_STATE_SHADER_READ, RZ_GFX_RESOURCE_STATE_RENDER_TARGET);

                    rz_gfx_renderpass info                 = {};
                    info.resolution                        = RZ_GFX_RESOLUTION_WINDOW;
                    info.colorAttachmentsCount             = 1;
                    info.colorAttachments[0].pResourceView = RZResourceManager::Get().getResourceViewResource(resources.getResourceViewHandle<RZFrameGraphTexture>(data.LDR));
                    info.colorAttachments[0].clear         = false;
                    info.colorAttachments[0].clearColor    = RAZIX_GFX_COLOR_RGBA_BLACK;
                    info.layers                            = 1;
                    RAZIX_X(info.extents)                  = RZApplication::Get().getWindow()->getWidth();
                    RAZIX_Y(info.extents)                  = RZApplication::Get().getWindow()->getHeight();
                    rzRHI_BeginRenderPass(cmdBuffer, &info);

                    rzRHI_BindGfxRootSig(cmdBuffer, m_RootSignature);
                    rzRHI_BindPipeline(cmdBuffer, m_Pipeline);

                    rz_gfx_descriptor_heap_handle heaps[] = {
                        RZEngine::Get().getWorldRenderer().getSamplerHeap(),
                        RZEngine::Get().getWorldRenderer().getResourceHeap(),
                    };
                    rzRHI_BindDescriptorHeaps(cmdBuffer, heaps, 2);

                    if (RZFrameGraph::IsFirstFrame()) {
                        RZResourceManager::Get()
                            .getShaderBindMapRef(RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::kTonemap))
                            .setDescriptorBlacklist(s_GlobalSamplersBlacklistPreset)
                            .setDescriptorBlacklist(s_RootConstantBlacklistPreset)
                            .setDescriptorTable(RZEngine::Get().getWorldRenderer().getGlobalSamplerTable())
                            .setResourceView("SceneHDRRenderTarget", resources.getResourceViewHandle<RZFrameGraphTexture>(sceneData.HDR))
                            .validate()
                            .build();
                    }

                    RZResourceManager::Get()
                        .getShaderBindMapRef(RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::kTonemap))
                        .bind(cmdBuffer, RZ_GFX_PIPELINE_TYPE_GRAPHICS);

                    // bind root constant to control the Tonemapping mode
                    struct PushConstant
                    {
                        u32 tonemapMode;
                    } m_PushConstantData           = {};
                    m_PushConstantData.tonemapMode = settings->tonemapMode;
                    rzRHI_BindRootConstant(cmdBuffer, RZ_GFX_PIPELINE_TYPE_GRAPHICS, m_RootSignature, 0, sizeof(PushConstant), &m_PushConstantData);

                    // Full screen quad, just 3 tris, generated in the vertex shader
                    rzRHI_DrawAuto(cmdBuffer, 3, 1, 0, 0);

                    rzRHI_EndRenderPass(cmdBuffer);

                    rzRHI_InsertImageBarrier(cmdBuffer, resources.get<RZFrameGraphTexture>(data.LDR).getRHIHandle(), RZ_GFX_RESOURCE_STATE_RENDER_TARGET, RZ_GFX_RESOURCE_STATE_SHADER_READ);

                    RAZIX_MARK_END(cmdBuffer);
                    RAZIX_TIME_STAMP_END();
                },
                [=]() {
                    RZResourceManager::Get()
                        .getShaderBindMapRef(RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::kTonemap))
                        .destroy();
                    RZResourceManager::Get().destroyPipeline(m_Pipeline);
                });
        }
    }    // namespace Gfx
}    // namespace Razix
