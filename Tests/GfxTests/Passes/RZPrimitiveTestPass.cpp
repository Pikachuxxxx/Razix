#include "RZPrimitiveTestPass.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"
#include "Razix/Scene/RZSceneCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Razix {
    namespace Gfx {

#define MAX_INSTANCES 16

        struct PrimitiveTestVertex
        {
            float px, py, pz;
            float nx, ny, nz;
        };

        struct PrimitiveInstanceData
        {
            float m[16];
        };

        struct PrimitiveCBData
        {
            glm::mat4 view;
            glm::mat4 projection;
            glm::mat4 instanceTransforms[MAX_INSTANCES];
        };

        static std::vector<PrimitiveTestVertex>   s_Vertices;
        static std::vector<u32>                   s_Indices;
        static std::vector<PrimitiveInstanceData> s_InstanceTransforms;

        void RZPrimitiveTestPass::createGeometryCPU()
        {
            s_Vertices.clear();
            s_Indices.clear();

            // clang-format off
            PrimitiveTestVertex baseVerts[8] = {
                {-0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f},
                { 0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f},
                { 0.5f,  0.5f, -0.5f, 0.f, 0.f, -1.f},
                {-0.5f,  0.5f, -0.5f, 0.f, 0.f, -1.f},
                {-0.5f, -0.5f,  0.5f, 0.f, 0.f,  1.f},
                { 0.5f, -0.5f,  0.5f, 0.f, 0.f,  1.f},
                { 0.5f,  0.5f,  0.5f, 0.f, 0.f,  1.f},
                {-0.5f,  0.5f,  0.5f, 0.f, 0.f,  1.f},
            };
            s_Vertices.insert(s_Vertices.end(), std::begin(baseVerts), std::end(baseVerts));

            u32 cubeIndices[] = {
                0, 1, 2, 2, 3, 0,
                4, 5, 6, 6, 7, 4,
                0, 3, 7, 7, 4, 0,
                1, 5, 6, 6, 2, 1,
                0, 4, 5, 5, 1, 0,
                3, 2, 6, 6, 7, 3
            };
            // clang-format on

            s_Indices.insert(s_Indices.end(), std::begin(cubeIndices), std::end(cubeIndices));

            m_NumVertices = static_cast<u32>(s_Vertices.size());
            m_NumIndices  = static_cast<u32>(s_Indices.size());

            rz_gfx_buffer_desc vbDesc = {};
            vbDesc.type               = RZ_GFX_BUFFER_TYPE_VERTEX;
            vbDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
            vbDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_NONE;
            vbDesc.sizeInBytes        = m_NumVertices * sizeof(PrimitiveTestVertex);
            vbDesc.pInitData          = s_Vertices.data();
            m_VertexBuffer            = RZResourceManager::Get().createBuffer("Buffer.PrimitiveTest.Vertices", vbDesc);

            rz_gfx_buffer_desc ibDesc = {};
            ibDesc.type               = RZ_GFX_BUFFER_TYPE_INDEX;
            ibDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
            ibDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_NONE;
            ibDesc.sizeInBytes        = m_NumIndices * sizeof(u32);
            ibDesc.pInitData          = s_Indices.data();
            m_IndexBuffer             = RZResourceManager::Get().createBuffer("Buffer.PrimitiveTest.Indices", ibDesc);
        }

        void RZPrimitiveTestPass::createInstanceDataCPU()
        {
            s_InstanceTransforms.clear();
            s_InstanceTransforms.resize(MAX_INSTANCES);

            for (u32 i = 0; i < MAX_INSTANCES; ++i) {
                // Calculate grid dimensions (assuming square grid)
                u32 gridSize = static_cast<u32>(sqrt(MAX_INSTANCES));
                u32 x        = i % gridSize;
                u32 y        = i / gridSize;

                // Define spacing between cubes (adjust as needed)
                float spacing = 2.5f;    // Gap between cube centers


                // Calculate position with proper centering
                float tx = (static_cast<float>(x) - static_cast<float>(gridSize - 1) * 0.5f) * spacing;
                float ty = (static_cast<float>(y) - static_cast<float>(gridSize - 1) * 0.5f) * spacing;
                float tz = 0.0f;

                // Create transformation matrix
                glm::mat4 model = glm::mat4(1.0f);
                model           = glm::translate(model, glm::vec3(tx, ty, tz));

                // Copy to instance data
                memcpy(s_InstanceTransforms[i].m, &model[0][0], sizeof(float) * 16);
            }
        }

        void RZPrimitiveTestPass::createIndirectArgs()
        {
            rz_gfx_draw_indexed_indirect_args args = {};
            args.indexCount                        = m_NumIndices;
            args.instanceCount                     = MAX_INSTANCES;
            args.firstIndex                        = 0;
            args.firstInstance                     = 0;
            args.vertexOffset                      = 0;

            rz_gfx_buffer_desc indirectDesc = {};
            indirectDesc.type               = RZ_GFX_BUFFER_TYPE_INDIRECT_ARGS;
            indirectDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
            indirectDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_SRV;
            indirectDesc.sizeInBytes        = sizeof(rz_gfx_draw_indexed_indirect_args);
            indirectDesc.pInitData          = &args;
            m_IndirectArgsBuffer            = RZResourceManager::Get().createBuffer("Buffer.PrimitiveTest.IndirectArgs", indirectDesc);
        }

        static PrimitiveCBData cbData = {};

        void RZPrimitiveTestPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            rz_gfx_shader_desc shaderDesc = {};
            shaderDesc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            shaderDesc.rzsfFilePath       = "//TestsRoot/GfxTests/Shaders/Razix/Shader.Test.PrimitiveTest.rzsf";
            m_Shader                      = RZResourceManager::Get().createShader("Shader.GfxTest.PrimitiveTest", shaderDesc);

            rz_gfx_pipeline_desc pipelineDesc   = {};
            pipelineDesc.type                   = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            pipelineDesc.pShader                = RZResourceManager::Get().getShaderResource(m_Shader);
            m_RootSigHandle                     = pipelineDesc.pShader->rootSignature;
            pipelineDesc.pRootSig               = RZResourceManager::Get().getRootSignatureResource(pipelineDesc.pShader->rootSignature);
            pipelineDesc.depthTestEnabled       = true;
            pipelineDesc.depthWriteEnabled      = true;
            pipelineDesc.cullMode               = RZ_GFX_CULL_MODE_TYPE_BACK;
            pipelineDesc.drawType               = RZ_GFX_DRAW_TYPE_TRIANGLE;
            pipelineDesc.blendEnabled           = false;
            pipelineDesc.useBlendPreset         = true;
            pipelineDesc.blendPreset            = RZ_GFX_BLEND_PRESET_ADDITIVE;
            pipelineDesc.renderTargetCount      = 1;
            pipelineDesc.renderTargetFormats[0] = RZ_GFX_FORMAT_SCREEN;
            pipelineDesc.depthStencilFormat     = RZ_GFX_FORMAT_D32_FLOAT;
            pipelineDesc.depthCompareOp         = RZ_GFX_COMPARE_OP_TYPE_LESS;
            pipelineDesc.inputLayoutMode        = RZ_GFX_INPUT_LAYOUT_AOS;
            m_Pipeline                          = RZResourceManager::Get().createPipeline("Pipeline.GfxTest.PrimitiveTest", pipelineDesc);

            createGeometryCPU();
            createInstanceDataCPU();
            createIndirectArgs();

            RZSceneCamera camera;
            u32           winW = RZApplication::Get().getWindow()->getWidth();
            u32           winH = RZApplication::Get().getWindow()->getHeight();
            camera.setViewportSize(winW, winH);
            camera.setPosition({0.0f, 0.0f, 15.0f});
            camera.setForward({0.0f, 0.0f, 1.0f});

            cbData.view       = camera.getViewMatrix();
            cbData.projection = camera.getProjection();
            for (u32 i = 0; i < MAX_INSTANCES; ++i) {
                memcpy(&cbData.instanceTransforms[i], s_InstanceTransforms[i].m, sizeof(float) * 16);
            }

            RZResourceManager::Get()
                .getShaderBindMapRef(m_Shader)
                .RegisterBindMap(m_Shader);

            struct PrimitivePassData
            {
                RZFrameGraphResource primitiveCB;
                RZFrameGraphResource depth;
            };

            framegraph.addCallbackPass<PrimitivePassData>(
                "[Test] Pass.Builtin.Code.PrimitiveDraw",
                [&](PrimitivePassData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    rz_gfx_texture_desc depthTexDesc = {};
                    depthTexDesc.width               = RZApplication::Get().getWindow()->getWidth();
                    depthTexDesc.height              = RZApplication::Get().getWindow()->getHeight();
                    depthTexDesc.depth               = 1;
                    depthTexDesc.arraySize           = 1;
                    depthTexDesc.mipLevels           = 1;
                    depthTexDesc.textureType         = RZ_GFX_TEXTURE_TYPE_2D;
                    depthTexDesc.format              = RZ_GFX_FORMAT_D32_FLOAT;
                    depthTexDesc.resourceHints       = RZ_GFX_RESOURCE_VIEW_FLAG_DSV;
                    data.depth                       = builder.create<RZFrameGraphTexture>("Texture.PrimitiveTest.Depth", CAST_TO_FG_TEX_DESC depthTexDesc);

                    rz_gfx_resource_view_desc dsvDesc      = {};
                    dsvDesc.descriptorType                 = RZ_GFX_DESCRIPTOR_TYPE_DEPTH_STENCIL_TEXTURE;
                    dsvDesc.textureViewDesc.pTexture       = RZ_FG_TEX_RES_AUTO_POPULATE;
                    dsvDesc.pRtvDsvHeap                    = RZResourceManager::Get().getDescriptorHeapResource(RZEngine::Get().getWorldRenderer().getDepthRenderTargetHeap());
                    dsvDesc.textureViewDesc.baseMip        = 0;
                    dsvDesc.textureViewDesc.baseArrayLayer = 0;
                    dsvDesc.textureViewDesc.dimension      = 1;
                    data.depth                             = builder.write(data.depth, dsvDesc);

                    rz_gfx_buffer_desc cbDesc = {};
                    cbDesc.type               = RZ_GFX_BUFFER_TYPE_CONSTANT;
                    cbDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_DYNAMIC;
                    cbDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_CBV;
                    cbDesc.sizeInBytes        = sizeof(PrimitiveCBData);
                    cbDesc.pInitData          = &cbData;
                    data.primitiveCB          = builder.create<RZFrameGraphBuffer>("Buffer.PrimitiveTest.InstanceTransforms", CAST_TO_FG_BUF_DESC cbDesc);

                    rz_gfx_resource_view_desc cbvDesc = {};
                    cbvDesc.descriptorType            = RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER;
                    cbvDesc.bufferViewDesc.pBuffer    = RZ_FG_BUF_RES_AUTO_POPULATE;
                    cbvDesc.bufferViewDesc.size       = sizeof(PrimitiveCBData);
                    cbvDesc.bufferViewDesc.offset     = 0;
                    data.primitiveCB                  = builder.write(data.primitiveCB, cbvDesc);
                },
                [=](const PrimitivePassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_TIME_STAMP_BEGIN("[Test] Primitive Draw Pass");

                    rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
                    RAZIX_MARK_BEGIN(cmdBuffer, "[Test] Pass.Builtin.Code.PrimitiveDraw", GenerateHashedColor4(1337u));

                    // Render pass targeting swapchain
                    rz_gfx_renderpass rp                 = {0};
                    rp.resolution                        = RZ_GFX_RESOLUTION_WINDOW;
                    rp.colorAttachmentsCount             = 1;
                    rp.colorAttachments[0].pResourceView = RZEngine::Get().getWorldRenderer().getCurrSwapchainBackbufferResViewPtr();
                    rp.colorAttachments[0].clear         = true;
                    rp.colorAttachments[0].clearColor    = {0.2f, 0.2f, 0.2f, 1.0f};
                    rp.depthAttachment.pResourceView     = RZResourceManager::Get().getResourceViewResource(resources.getResourceViewHandle<RZFrameGraphTexture>(data.depth));
                    rp.depthAttachment.clear             = true;
                    rp.layers                            = 1;
                    RAZIX_X(rp.extents)                  = RZApplication::Get().getWindow()->getWidth();
                    RAZIX_Y(rp.extents)                  = RZApplication::Get().getWindow()->getHeight();

                    rzRHI_BeginRenderPass(cmdBuffer, &rp);
                    rzRHI_BindGfxRootSig(cmdBuffer, m_RootSigHandle);
                    rzRHI_BindPipeline(cmdBuffer, m_Pipeline);

                    const rz_gfx_descriptor_heap_handle heaps[] = {RZEngine::Get().getWorldRenderer().getResourceHeap()};
                    rzRHI_BindDescriptorHeaps(cmdBuffer, heaps, 1);

                    if (RZFrameGraph::IsFirstFrame()) {
                        RZResourceManager::Get()
                            .getShaderBindMapRef(m_Shader)
                            .setResourceView("InstanceTransforms", resources.getResourceViewHandle<RZFrameGraphBuffer>(data.primitiveCB))
                            .validate()
                            .build();
                    }

                    RZResourceManager::Get()
                        .getShaderBindMapRef(m_Shader)
                        .bind(cmdBuffer, RZ_GFX_PIPELINE_TYPE_GRAPHICS);

                    rz_gfx_buffer_update cbUpdate = {};
                    cbUpdate.pBuffer              = RZResourceManager::Get().getBufferResource(resources.get<RZFrameGraphBuffer>(data.primitiveCB).getRHIHandle());
                    cbUpdate.sizeInBytes          = sizeof(PrimitiveCBData);
                    cbUpdate.offset               = 0;
                    cbUpdate.pData                = &cbData;
                    rzRHI_UpdateConstantBuffer(cbUpdate);

                    // Bind vertex and index buffers
                    u32                  offsets[] = {0};
                    u32                  strides[] = {sizeof(PrimitiveTestVertex)};
                    rz_gfx_buffer_handle vbs[]     = {m_VertexBuffer};
                    rzRHI_BindVertexBuffers(cmdBuffer, vbs, 1, offsets, strides);
                    rzRHI_BindIndexBuffer(cmdBuffer, m_IndexBuffer, 0, RZ_GFX_INDEX_TYPE_UINT32);

                    rzRHI_DrawIndexedIndirect(cmdBuffer, m_IndirectArgsBuffer, 0, 1);

                    rzRHI_EndRenderPass(cmdBuffer);

                    RAZIX_MARK_END(cmdBuffer);
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZPrimitiveTestPass::destroy()
        {
            RZResourceManager::Get().getShaderBindMapRef(m_Shader).destroy();
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            RZResourceManager::Get().destroyShader(m_Shader);
            RZResourceManager::Get().destroyBuffer(m_VertexBuffer);
            RZResourceManager::Get().destroyBuffer(m_IndexBuffer);
            RZResourceManager::Get().destroyBuffer(m_IndirectArgsBuffer);
        }

    }    // namespace Gfx
}    // namespace Razix
