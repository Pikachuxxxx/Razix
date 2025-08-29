#include "RZPrimitiveTestPass.h"

#include "Razix/Gfx/RHI/RHI.h"
#include "Razix/Gfx/Resources/RZResourceManager.h"

namespace Razix {
    namespace Gfx {

        // Simple cube vertex structure (position + normal + uv). Adjust semantic names to match shader expectations later.
        struct PrimitiveTestVertex
        {
            float px, py, pz;   // position
            float nx, ny, nz;   // normal
            float u, v;         // uv
        };

        // Instance transform data (could be full 4x4 matrix; to keep size reasonable we store 3 rows of mat4, last row reconstructed or store full 4x4 if needed)
        struct PrimitiveInstanceData
        {
            float m[16]; // Placeholder 4x4 matrix. User can optimize layout / use MAT4 array in shader.
        };

        // CPU staging arrays (scaffolding). In a fuller implementation these would likely be members with proper lifetime mgmt.
        static std::vector<PrimitiveTestVertex> s_Vertices; // Filled in createGeometryCPU
        static std::vector<u32>                 s_Indices;  // Filled in createGeometryCPU
        static std::vector<PrimitiveInstanceData> s_InstanceTransforms; // Filled in createInstanceDataCPU

        void RZPrimitiveTestPass::createGeometryCPU()
        {
            // Create a unit cube (positions + normals). UVs are placeholder.
            // NOTE: For brevity, normals are approximate and duplicated per face if needed. User can refine.
            s_Vertices.clear();
            s_Indices.clear();

            // Cube 8 unique verts (could duplicate per-face for flat shading). We'll keep it simple.
            struct V { float x,y,z; float nx,ny,nz; float u,v; };
            // Positions and normals placeholder; user can adjust for proper face normals or expand to 24 verts.
            V baseVerts[8] = {
                {-0.5f,-0.5f,-0.5f, 0.f,0.f,-1.f, 0.f,0.f},
                { 0.5f,-0.5f,-0.5f, 0.f,0.f,-1.f, 1.f,0.f},
                { 0.5f, 0.5f,-0.5f, 0.f,0.f,-1.f, 1.f,1.f},
                {-0.5f, 0.5f,-0.5f, 0.f,0.f,-1.f, 0.f,1.f},
                {-0.5f,-0.5f, 0.5f, 0.f,0.f, 1.f, 0.f,0.f},
                { 0.5f,-0.5f, 0.5f, 0.f,0.f, 1.f, 1.f,0.f},
                { 0.5f, 0.5f, 0.5f, 0.f,0.f, 1.f, 1.f,1.f},
                {-0.5f, 0.5f, 0.5f, 0.f,0.f, 1.f, 0.f,1.f},
            };
            for (auto& v : baseVerts) {
                PrimitiveTestVertex vx { v.x,v.y,v.z, v.nx,v.ny,v.nz, v.u,v.v };
                s_Vertices.push_back(vx);
            }
            // 12 triangles (two per face) using 8 shared verts (indexed cube)
            u32 cubeIndices[] = {
                // front (-z)
                0,1,2, 2,3,0,
                // back (+z)
                4,5,6, 6,7,4,
                // left (-x)
                0,3,7, 7,4,0,
                // right (+x)
                1,5,6, 6,2,1,
                // bottom (-y)
                0,4,5, 5,1,0,
                // top (+y)
                3,2,6, 6,7,3
            };
            s_Indices.insert(s_Indices.end(), std::begin(cubeIndices), std::end(cubeIndices));

            m_NumVertices = static_cast<u32>(s_Vertices.size());
            m_NumIndices  = static_cast<u32>(s_Indices.size());

            // Create vertex buffer
            rz_gfx_buffer_desc vbDesc = {};
            vbDesc.type               = RZ_GFX_BUFFER_TYPE_BYTE; // Placeholder: engine likely has a specific vertex buffer type; adjust if needed.
            vbDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
            vbDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_SRV; // Not necessarily needed; placeholder.
            vbDesc.sizeInBytes        = m_NumVertices * sizeof(PrimitiveTestVertex);
            m_VertexBuffer            = RZResourceManager::Get().createBuffer("Buffer.PrimitiveTest.Vertices", vbDesc, s_Vertices.data()); // If overload not present, user must upload via update call.
            // If createBuffer overload with initial data not available, user: TODO implement staging upload.

            // Create index buffer
            rz_gfx_buffer_desc ibDesc = {};
            ibDesc.type               = RZ_GFX_BUFFER_TYPE_BYTE; // Placeholder again.
            ibDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
            ibDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_SRV; // Placeholder (index buffers typically not SRV; depends on engine design).
            ibDesc.sizeInBytes        = m_NumIndices * sizeof(u32);
            m_IndexBuffer             = RZResourceManager::Get().createBuffer("Buffer.PrimitiveTest.Indices", ibDesc, s_Indices.data());
            // TODO: If index buffer binding requires special desc flag, adjust (no info found in RHI.h for dedicated index buffer flag).
        }

        void RZPrimitiveTestPass::createInstanceDataCPU()
        {
            s_InstanceTransforms.clear();
            m_NumInstances = 16; // Example grid 4x4; user can tweak.
            s_InstanceTransforms.resize(m_NumInstances);

            // Generate simple transforms: translate cubes in a grid.
            for (u32 i = 0; i < m_NumInstances; ++i) {
                float tx = static_cast<float>(i % 4) - 1.5f;
                float ty = 0.0f;
                float tz = static_cast<float>(i / 4) - 1.5f;
                PrimitiveInstanceData inst{};
                // Row-major placeholder identity with translation last column (adjust for engine's matrix convention!)
                // TODO: Confirm column-major vs row-major for shader consumption. Using simple identity + translation.
                for (int r = 0; r < 16; ++r) inst.m[r] = 0.0f;
                inst.m[0] = inst.m[5] = inst.m[10] = inst.m[15] = 1.0f;
                inst.m[12] = tx; inst.m[13] = ty; inst.m[14] = tz; // Assume last row for translation; user verify.
                s_InstanceTransforms[i] = inst;
            }

            // Create / upload instance buffer
            rz_gfx_buffer_desc instDesc = {};
            instDesc.type               = RZ_GFX_BUFFER_TYPE_STRUCTURED; // Using structured for per-instance data
            instDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_DYNAMIC; // Might update per frame
            instDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_SRV; // If shader reads as structured buffer
            instDesc.sizeInBytes        = static_cast<u32>(s_InstanceTransforms.size() * sizeof(PrimitiveInstanceData));
            instDesc.stride             = sizeof(PrimitiveInstanceData);
            instDesc.elementCount       = m_NumInstances;
            m_InstanceBuffer            = RZResourceManager::Get().createBuffer("Buffer.PrimitiveTest.Instances", instDesc, s_InstanceTransforms.data());
            // TODO: If instancing uses per-vertex input rate instead, need to supply second vertex stream via input layout (not shown yet)
        }

        void RZPrimitiveTestPass::createIndirectArgs()
        {
            // Prepare a rz_gfx_draw_indirect_args structure.
            rz_gfx_draw_indirect_args args = {};
            args.vertexCount   = m_NumIndices;   // Or number of vertices if non-indexed (we intend indexed; adapt later)
            args.instanceCount = m_NumInstances;
            args.firstVertex   = 0;
            args.firstInstance = 0;

            rz_gfx_buffer_desc indirectDesc = {};
            indirectDesc.type               = RZ_GFX_BUFFER_TYPE_BYTE; // No dedicated type for indirect args in current enum, using BYTE as placeholder.
            indirectDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_INDIRECT_DRAW_ARGS; // Usage hints we DO have.
            indirectDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_SRV; // Placeholder (actual binding may not need SRV)
            indirectDesc.sizeInBytes        = sizeof(rz_gfx_draw_indirect_args);
            m_IndirectArgsBuffer            = RZResourceManager::Get().createBuffer("Buffer.PrimitiveTest.IndirectArgs", indirectDesc, &args);
            // NOTE: Actual DrawIndirect RHI call not defined yet (see TODOs in RHI.h); user will replace DrawAuto with DrawIndirect when available.
        }

        void RZPrimitiveTestPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* /*scene*/, RZRendererSettings* /*settings*/)
        {
            // 1. Load shader (VS+PS) from new rzsf file.
            rz_gfx_shader_desc shaderDesc = {};
            shaderDesc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            shaderDesc.rzsfFilePath       = "//TestsRoot/GfxTests/PrimitiveTest/Shaders/Razix/Shader.Tests.DrawPrimitive.rzsf"; // Path must match new file
            m_Shader                      = RZResourceManager::Get().createShader("Shader.GfxTest.DrawPrimitive", shaderDesc);

            // 2. Build pipeline descriptor. Input layout TBD (we rely on shader reflection to populate). If manual layout needed, fill shaderDesc.pElements.
            rz_gfx_pipeline_desc pipelineDesc = {};
            pipelineDesc.type                 = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            pipelineDesc.pShader              = RZResourceManager::Get().getShaderResource(m_Shader);
            m_RootSigHandle                   = pipelineDesc.pShader->rootSignature;
            pipelineDesc.pRootSig             = RZResourceManager::Get().getRootSignatureResource(pipelineDesc.pShader->rootSignature);
            pipelineDesc.depthTestEnabled     = false; // Optional enable later if needed.
            pipelineDesc.depthWriteEnabled    = false;
            pipelineDesc.cullMode             = RZ_GFX_CULL_MODE_TYPE_BACK; // Could be NONE for debug
            pipelineDesc.drawType             = RZ_GFX_DRAW_TYPE_TRIANGLE;
            pipelineDesc.blendEnabled         = false;
            pipelineDesc.useBlendPreset       = true;
            pipelineDesc.blendPreset          = RZ_GFX_BLEND_PRESET_ADDITIVE; // Not critical.
            pipelineDesc.renderTargetCount    = 1;
            pipelineDesc.renderTargetFormats[0] = RZ_GFX_FORMAT_SCREEN;
            m_Pipeline = RZResourceManager::Get().createPipeline("Pipeline.GfxTest.DrawPrimitive", pipelineDesc);

            // 3. Create geometry + instance + indirect args
            createGeometryCPU();
            createInstanceDataCPU();
            createIndirectArgs();

            // Register shader bind map for resource bindings (instance buffer SRV / camera matrices constant buffer etc.)
            RZResourceManager::Get().getShaderBindMap(m_Shader).RegisterBindMap(m_Shader);

            struct PrimitivePassData
            {
                // If we needed per-pass constant buffers or render targets, declare them here.
                // For now we just render straight to the swapchain.
                // Example placeholder for view/projection constant buffer if needed later:
                // RZFrameGraphResource CameraCB;
            };

            framegraph.addCallbackPass<PrimitivePassData>(
                "[Test] Pass.Builtin.Code.PrimitiveDraw",
                [&](PrimitivePassData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    // TODO: Create constant buffer (camera + projection) and write view if needed.
                    // rz_gfx_buffer_desc cameraCBDesc = {};
                    // cameraCBDesc.type = RZ_GFX_BUFFER_TYPE_CONSTANT;
                    // cameraCBDesc.usage = RZ_GFX_BUFFER_USAGE_TYPE_DYNAMIC;
                    // cameraCBDesc.resourceHints = RZ_GFX_RESOURCE_VIEW_FLAG_CBV;
                    // cameraCBDesc.sizeInBytes = sizeof(CameraMatricesStruct); // define struct
                    // data.CameraCB = builder.create<RZFrameGraphBuffer>("CBuffer.GfxTest.PrimitiveCamera", CAST_TO_FG_BUF_DESC cameraCBDesc);
                    // rz_gfx_resource_view_desc cbView = {}; cbView.descriptorType = RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER; cbView.bufferViewDesc.pBuffer = RZ_FG_BUF_RES_AUTO_POPULATE; cbView.bufferViewDesc.size = cameraCBDesc.sizeInBytes; data.CameraCB = builder.write(data.CameraCB, cbView);
                },
                [=](const PrimitivePassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_TIME_STAMP_BEGIN("[Test] Primitive Draw Pass");

                    rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
                    RAZIX_MARK_BEGIN(cmdBuffer, "[Test] Pass.Builtin.Code.PrimitiveDraw", Utilities::GenerateHashedColor4(1337u));

                    // Render pass targeting swapchain
                    rz_gfx_renderpass rp = {0};
                    rp.resolution                        = RZ_GFX_RESOLUTION_WINDOW;
                    rp.colorAttachmentsCount             = 1;
                    rp.colorAttachments[0].pResourceView = RZEngine::Get().getWorldRenderer().getCurrSwapchainBackbufferResViewPtr();
                    rp.colorAttachments[0].clear         = true;
                    rp.layers                            = 1;
                    RAZIX_X(rp.extents)                  = RZApplication::Get().getWindow()->getWidth();
                    RAZIX_Y(rp.extents)                  = RZApplication::Get().getWindow()->getHeight();

                    rzRHI_BeginRenderPass(cmdBuffer, &rp);
                    rzRHI_BindGfxRootSig(cmdBuffer, m_RootSigHandle);
                    rzRHI_BindPipeline(cmdBuffer, m_Pipeline);

                    // Bind descriptor heaps (resource heap for buffers). If sampler heap needed add similarly.
                    rz_gfx_descriptor_heap_handle heaps[] = { RZEngine::Get().getWorldRenderer().getResourceHeap() };
                    rzRHI_BindDescriptorHeaps(cmdBuffer, heaps, 1);

                    if (RZFrameGraph::IsFirstFrame()) {
                        // Bind instance buffer as SRV (assuming shader expects structured buffer named g_InstanceTransforms)
                        RZResourceManager::Get()
                            .getShaderBindMap(m_Shader)
                            // .setResourceView("g_CameraCB", resources.getResourceViewHandle<RZFrameGraphBuffer>(data.CameraCB)) // Uncomment if camera CB added
                            .setResourceView("g_InstanceTransforms", RZResourceManager::Get().createResourceViewFromBuffer(m_InstanceBuffer /*TODO: if helper exists; else create separately*/)) // Placeholder helper call (verify actual API). If unavailable, user must create resource view manually earlier.
                            .validate()
                            .build();
                    }

                    RZResourceManager::Get().getShaderBindMap(m_Shader).bind(cmdBuffer, RZ_GFX_PIPELINE_TYPE_GRAPHICS);

                    // TODO: Bind vertex + index buffers. RHI.h does not yet expose explicit BindVertexBuffers/BindIndexBuffer in provided snippet.
                    // Placeholder comment: User to add binding calls once available (e.g., rzRHI_BindVertexBuffer / rzRHI_BindIndexBuffer).

                    // For now use DrawAuto as placeholder (non-indexed). Replace with DrawIndexedAuto or DrawIndirect when implemented.
                    rzRHI_DrawAuto(cmdBuffer, m_NumVertices, m_NumInstances, 0, 0);

                    // TODO: Replace above with DrawIndirect using m_IndirectArgsBuffer once rzRHI_DrawIndirect is implemented in RHI.

                    rzRHI_EndRenderPass(cmdBuffer);

                    RAZIX_MARK_END(cmdBuffer);
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZPrimitiveTestPass::destroy()
        {
            // Destroy descriptor tables created by bind map
            RZResourceManager::Get().getShaderBindMap(m_Shader).destroy();
            // Destroy pipeline + shader
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            RZResourceManager::Get().destroyShader(m_Shader);
            // Destroy buffers (if resource manager requires explicit destruction)
            if (m_VertexBuffer.handle)       RZResourceManager::Get().destroyBuffer(m_VertexBuffer);
            if (m_IndexBuffer.handle)        RZResourceManager::Get().destroyBuffer(m_IndexBuffer);
            if (m_InstanceBuffer.handle)     RZResourceManager::Get().destroyBuffer(m_InstanceBuffer);
            if (m_IndirectArgsBuffer.handle) RZResourceManager::Get().destroyBuffer(m_IndirectArgsBuffer);
        }

    } // namespace Gfx
} // namespace Razix
