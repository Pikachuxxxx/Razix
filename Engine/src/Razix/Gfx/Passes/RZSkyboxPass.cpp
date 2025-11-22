// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSkyboxPass.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZMeshFactory.h"
#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Passes/GlobalData.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"
#include "Razix/Gfx/Resources/RZResourceManager.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

#define NUM_SKYBOX_VERTICES 8
#define NUM_SKYBOX_INDICES  36

namespace Razix {
    namespace Gfx {

        struct SkyboxCubeVertex
        {
            float px, py, pz;
        };

        static RZDynamicArray<SkyboxCubeVertex> s_Vertices;
        static RZDynamicArray<u32>              s_Indices;

        static void CreateSkyboxGeometry(rz_gfx_buffer_handle& vb, rz_gfx_buffer_handle& ib)
        {
            s_Vertices.clear();
            s_Indices.clear();

            // clang-format off
            SkyboxCubeVertex baseVerts[NUM_SKYBOX_VERTICES] = {
                {-0.5f, -0.5f, -0.5f},
                { 0.5f, -0.5f, -0.5f},
                { 0.5f,  0.5f, -0.5f},
                {-0.5f,  0.5f, -0.5f},
                {-0.5f, -0.5f,  0.5f},
                { 0.5f, -0.5f,  0.5f},
                { 0.5f,  0.5f,  0.5f},
                {-0.5f,  0.5f,  0.5f},
            };
            for (u32 i = 0; i < NUM_SKYBOX_VERTICES; ++i) {
                s_Vertices.push_back(baseVerts[i]);
            }

            u32 cubeIndices[NUM_SKYBOX_INDICES] = {
                0, 1, 2, 2, 3, 0,
                4, 5, 6, 6, 7, 4,
                0, 3, 7, 7, 4, 0,
                1, 5, 6, 6, 2, 1,
                0, 4, 5, 5, 1, 0,
                3, 2, 6, 6, 7, 3
            };
            for (u32 i = 0; i < NUM_SKYBOX_INDICES; ++i) {
                s_Indices.push_back(cubeIndices[i]);
            }
            // clang-format on

            rz_gfx_buffer_desc vbDesc = {};
            vbDesc.type               = RZ_GFX_BUFFER_TYPE_VERTEX;
            vbDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
            vbDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_NONE;
            vbDesc.sizeInBytes        = NUM_SKYBOX_VERTICES * sizeof(SkyboxCubeVertex);
            vbDesc.pInitData          = s_Vertices.data();
            vb                        = RZResourceManager::Get().createBuffer("VB.Skybox", vbDesc);

            rz_gfx_buffer_desc ibDesc = {};
            ibDesc.type               = RZ_GFX_BUFFER_TYPE_INDEX;
            ibDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
            ibDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_NONE;
            ibDesc.sizeInBytes        = NUM_SKYBOX_INDICES * sizeof(u32);
            ibDesc.pInitData          = s_Indices.data();
            ib                        = RZResourceManager::Get().createBuffer("IB.Skybox", ibDesc);
        }

        void RZSkyboxPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            CreateSkyboxGeometry(m_VertexBuffer, m_IndexBuffer);

            m_SkyboxShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::kSkybox);
            // Optional procedural shader path remains unchanged
            // auto proceduralSkyboxShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::kProceduralSkybox);

            rz_gfx_pipeline_desc pipelineDesc   = {};
            pipelineDesc.pShader                = RZResourceManager::Get().getShaderResource(m_SkyboxShader);
            m_SkyboxRootSigHandle               = pipelineDesc.pShader->rootSignature;
            pipelineDesc.pRootSig               = RZResourceManager::Get().getRootSignatureResource(m_SkyboxRootSigHandle);
            pipelineDesc.type                   = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            pipelineDesc.cullMode               = RZ_GFX_CULL_MODE_TYPE_NONE;
            pipelineDesc.polygonMode            = RZ_GFX_POLYGON_MODE_TYPE_SOLID;
            pipelineDesc.depthTestEnabled       = false;    // TODO: Disable until scene rendering is done, we have no depth at all to test against yet!
            pipelineDesc.depthWriteEnabled      = false;    // Skybox usually doesn't write to depth
            pipelineDesc.depthCompareOp         = RZ_GFX_COMPARE_OP_TYPE_LESS_OR_EQUAL;
            pipelineDesc.drawType               = RZ_GFX_DRAW_TYPE_TRIANGLE;
            pipelineDesc.blendEnabled           = false;    // Disable blending for skybox
            pipelineDesc.blendPreset            = RZ_GFX_BLEND_PRESET_ADDITIVE;
            pipelineDesc.useBlendPreset         = true;
            pipelineDesc.renderTargetCount      = 1;
            pipelineDesc.renderTargetFormats[0] = RZ_GFX_FORMAT_R16G16B16A16_FLOAT;
            pipelineDesc.depthStencilFormat     = RZ_GFX_FORMAT_D16_UNORM;
            pipelineDesc.inputLayoutMode        = RZ_GFX_INPUT_LAYOUT_AOS;
            m_SkyboxPipeline                    = RZResourceManager::Get().createPipeline("Pipeline.Skybox", pipelineDesc);

            auto& frameDataBlock  = framegraph.getBlackboard().get<FrameData>();
            auto& lightProbesData = framegraph.getBlackboard().get<GlobalLightProbeData>();

            // Register the bind map for the skybox shader in RZResourceManager
            RZShaderBindMap::RegisterBindMap(m_SkyboxShader);

            framegraph.getBlackboard().add<SceneData>() = framegraph.addCallbackPass<SceneData>(
                "Pass.Builtin.Code.Skybox",
                [&](SceneData& data, RZPassResourceBuilder& builder) {
                    builder
                        .setAsStandAlonePass()
                        .setDepartment(Department::Environment);

                    rz_gfx_texture_desc sceneHDRTextureDesc         = {};
                    sceneHDRTextureDesc.width                       = RZApplication::Get().getWindow()->getWidth();
                    sceneHDRTextureDesc.height                      = RZApplication::Get().getWindow()->getHeight();
                    sceneHDRTextureDesc.depth                       = 1;
                    sceneHDRTextureDesc.format                      = RZ_GFX_FORMAT_R16G16B16A16_FLOAT;
                    sceneHDRTextureDesc.textureType                 = RZ_GFX_TEXTURE_TYPE_2D;
                    sceneHDRTextureDesc.mipLevels                   = 1;
                    sceneHDRTextureDesc.resourceHints               = rz_gfx_resource_view_hints(RZ_GFX_RESOURCE_VIEW_FLAG_RTV | RZ_GFX_RESOURCE_VIEW_FLAG_SRV);
                    data.HDR                                        = builder.create<RZFrameGraphTexture>("FG.Tex.SceneHDR", CAST_TO_FG_TEX_DESC sceneHDRTextureDesc);
                    rz_gfx_resource_view_desc sceneHDRViewDesc      = {};
                    sceneHDRViewDesc.descriptorType                 = RZ_GFX_DESCRIPTOR_TYPE_RENDER_TEXTURE;
                    sceneHDRViewDesc.textureViewDesc.pTexture       = RZ_FG_TEX_RES_AUTO_POPULATE;
                    sceneHDRViewDesc.textureViewDesc.baseMip        = 0;
                    sceneHDRViewDesc.textureViewDesc.baseArrayLayer = 0;
                    sceneHDRViewDesc.textureViewDesc.dimension      = 1;
                    sceneHDRViewDesc.pRtvDsvHeap                    = RZResourceManager::Get().getDescriptorHeapResource(RZEngine::Get().getWorldRenderer().getRenderTargetHeap());
                    data.HDR                                        = builder.write(data.HDR, sceneHDRViewDesc);

                    rz_gfx_texture_desc depthTextureDesc            = {};
                    depthTextureDesc.width                          = RZApplication::Get().getWindow()->getWidth();
                    depthTextureDesc.height                         = RZApplication::Get().getWindow()->getHeight();
                    depthTextureDesc.depth                          = 1;
                    depthTextureDesc.format                         = RZ_GFX_FORMAT_D16_UNORM;
                    depthTextureDesc.textureType                    = RZ_GFX_TEXTURE_TYPE_2D;
                    depthTextureDesc.mipLevels                      = 1;
                    depthTextureDesc.resourceHints                  = RZ_GFX_RESOURCE_VIEW_FLAG_DSV;
                    data.depth                                      = builder.create<RZFrameGraphTexture>("FG.Tex.SceneDepth", CAST_TO_FG_TEX_DESC depthTextureDesc);
                    rz_gfx_resource_view_desc depthTexViewDesc      = {};
                    depthTexViewDesc.descriptorType                 = RZ_GFX_DESCRIPTOR_TYPE_DEPTH_STENCIL_TEXTURE;
                    depthTexViewDesc.textureViewDesc.pTexture       = RZ_FG_TEX_RES_AUTO_POPULATE;
                    depthTexViewDesc.textureViewDesc.baseMip        = 0;
                    depthTexViewDesc.textureViewDesc.baseArrayLayer = 0;
                    depthTexViewDesc.textureViewDesc.dimension      = 1;
                    depthTexViewDesc.pRtvDsvHeap                    = RZResourceManager::Get().getDescriptorHeapResource(RZEngine::Get().getWorldRenderer().getDepthRenderTargetHeap());
                    data.depth                                      = builder.write(data.depth, depthTexViewDesc);

                    rz_gfx_resource_view_desc envMapTexViewDesc      = {};
                    envMapTexViewDesc.descriptorType                 = RZ_GFX_DESCRIPTOR_TYPE_TEXTURE;
                    envMapTexViewDesc.textureViewDesc.pTexture       = RZ_FG_TEX_RES_AUTO_POPULATE;
                    envMapTexViewDesc.textureViewDesc.baseMip        = 0;
                    envMapTexViewDesc.textureViewDesc.baseArrayLayer = 0;
                    envMapTexViewDesc.textureViewDesc.dimension      = 1;
                    builder.read(lightProbesData.environmentMap, envMapTexViewDesc);
                },
                [=](const SceneData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RETURN_IF_BIT_NOT_SET(settings->renderFeatures, RendererFeature_Skybox);

                    RAZIX_TIME_STAMP_BEGIN("Skybox Pass");
                    rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
                    RAZIX_MARK_BEGIN(cmdBuffer, "Skybox pass", float4(0.33f, 0.45f, 1.0f, 1.0f));

                    rzRHI_InsertImageBarrier(cmdBuffer, resources.get<RZFrameGraphTexture>(data.HDR).getRHIHandle(), RZ_GFX_RESOURCE_STATE_SHADER_READ, RZ_GFX_RESOURCE_STATE_RENDER_TARGET);

                    rz_gfx_renderpass info                 = {};
                    info.resolution                        = RZ_GFX_RESOLUTION_WINDOW;
                    info.colorAttachmentsCount             = 1;
                    info.colorAttachments[0].pResourceView = RZResourceManager::Get().getResourceViewResource(resources.getResourceViewHandle<RZFrameGraphTexture>(data.HDR));
                    info.colorAttachments[0].clear         = true;
                    info.colorAttachments[0].clearColor    = RAZIX_GFX_COLOR_RGBA_BLACK;
                    info.depthAttachment.pResourceView     = RZResourceManager::Get().getResourceViewResource(resources.getResourceViewHandle<RZFrameGraphTexture>(data.depth));
                    info.depthAttachment.clear             = false;
                    info.depthAttachment.clearColor        = {1.0f, 0.0f, 0.0f, 0.0f};
                    info.layers                            = 1;
                    RAZIX_X(info.extents)                  = RZApplication::Get().getWindow()->getWidth();
                    RAZIX_Y(info.extents)                  = RZApplication::Get().getWindow()->getHeight();

                    rzRHI_BeginRenderPass(cmdBuffer, &info);

                    rzRHI_BindGfxRootSig(cmdBuffer, m_SkyboxRootSigHandle);
                    rzRHI_BindPipeline(cmdBuffer, m_SkyboxPipeline);

                    rz_gfx_descriptor_heap_handle heaps[] = {
                        RZEngine::Get().getWorldRenderer().getSamplerHeap(),
                        RZEngine::Get().getWorldRenderer().getResourceHeap(),
                    };
                    rzRHI_BindDescriptorHeaps(cmdBuffer, heaps, 2);

                    if (RZFrameGraph::IsFirstFrame()) {
                        RZResourceManager::Get()
                            .getShaderBindMapRef(RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::kSkybox))
                            .setDescriptorTable(RZEngine::Get().getWorldRenderer().getFrameDataTable())
                            .setDescriptorTable(RZEngine::Get().getWorldRenderer().getGlobalSamplerTable())
                            .setDescriptorBlacklist(s_SystemDescriptorsBlacklistPreset)
                            .setDescriptorBlacklist(s_GlobalSamplersBlacklistPreset)
                            .setResourceView("EnvironmentMap", resources.getResourceViewHandle<RZFrameGraphTexture>(lightProbesData.environmentMap))
                            .validate()
                            .build();
                    }

                    RZResourceManager::Get()
                        .getShaderBindMapRef(RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::kSkybox))
                        .bind(cmdBuffer, RZ_GFX_PIPELINE_TYPE_GRAPHICS);

                    u32                  offsets[] = {0};
                    u32                  strides[] = {sizeof(SkyboxCubeVertex)};
                    rz_gfx_buffer_handle vbs[]     = {m_VertexBuffer};
                    rzRHI_BindVertexBuffers(cmdBuffer, vbs, 1, offsets, strides);
                    rzRHI_BindIndexBuffer(cmdBuffer, m_IndexBuffer, 0, RZ_GFX_INDEX_TYPE_UINT32);

                    rzRHI_DrawIndexedAuto(cmdBuffer, NUM_SKYBOX_INDICES, 1, 0, 0, 0);

                    rzRHI_EndRenderPass(cmdBuffer);

                    RAZIX_MARK_END(cmdBuffer);
                    RAZIX_TIME_STAMP_END();
                },
                [=]() {
                    RZResourceManager::Get()
                        .getShaderBindMapRef(m_SkyboxShader)
                        .destroy();
                    RZResourceManager::Get().destroyPipeline(m_SkyboxPipeline);
                    RZResourceManager::Get().destroyBuffer(m_VertexBuffer);
                    RZResourceManager::Get().destroyBuffer(m_IndexBuffer);
                });
        }
    }    // namespace Gfx
}    // namespace Razix
