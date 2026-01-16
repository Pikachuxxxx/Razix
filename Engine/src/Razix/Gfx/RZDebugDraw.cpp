// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZDebugDraw.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

#include "Razix/Gfx/Cameras/RZCamera3D.h"

namespace Razix {
    namespace Gfx {

        struct DebugLine
        {
            float3 p1;
            float  _pad0 = 0;
            float3 p2;
            float  _pad1 = 0;
            float4 col;

            DebugLine(const float3& pos1, const float3& pos2, const float4& colour)
            {
                p1  = pos1;
                p2  = pos2;
                col = colour;
            }
        };

        struct DebugPoint
        {
            float3 p1;
            float  size;
            float4 col;

            DebugPoint(const float3& pos1, float s, const float4& colour)
            {
                p1   = pos1;
                size = s;
                col  = colour;
            }
        };

        static const u32 MaxQuadVerts                  = 4;
        static const u32 MaxLineVerts                  = 2;
        static const u32 MaxPoints                     = 10000;
        static const u32 MaxPointVertices              = MaxPoints * MaxQuadVerts;    // 4 vertices per quad
        static const u32 MaxPointIndices               = MaxPoints * 6;               // 6 indices per quad
        static const u32 PointPositionDataSize         = sizeof(float4);
        static const u32 PointColorDataSize            = sizeof(float4);
        static const u32 PointPositionVertexBufferSize = PointPositionDataSize * MaxPointVertices;
        static const u32 PointColorVertexBufferSize    = PointColorDataSize * MaxPointVertices;
        static const u32 MaxLines                      = 10000;
        static const u32 MaxLineVertices               = MaxLines * MaxLineVerts;    // 2 vertices per line
        static const u32 MaxLineIndices                = MaxLines * 2;               // 6 indices per quad since we draw a line as a very thin quad
        static const u32 LinePositionDataSize          = sizeof(float4);
        static const u32 LineColorDataSize             = sizeof(float4);
        static const u32 LinePositionVertexBufferSize  = LinePositionDataSize * MaxLineVertices;
        static const u32 LineColorVertexBufferSize     = LineColorDataSize * MaxLineVertices;

        //---------------------------------------------------------------------------------------------------------------

        struct DebugDrawList
        {
            RZDynamicArray<DebugLine>  m_DebugLines  = {};
            RZDynamicArray<DebugPoint> m_DebugPoints = {};
        };

        struct DebugDrawState
        {
            DebugDrawList                drawList        = {};
            rz_gfx_pipeline_handle       linePipeline    = {};
            rz_gfx_pipeline_handle       pointPipeline   = {};
            rz_gfx_root_signature_handle lineRootSig     = {};
            rz_gfx_root_signature_handle pointRootSig    = {};
            rz_gfx_shader_handle         lineShader      = {};
            rz_gfx_shader_handle         pointShader     = {};
            rz_gfx_buffer_handle         lineIB          = {};    // index buffer
            rz_gfx_buffer_handle         linePosVB       = {};    // position vertex buffer
            rz_gfx_buffer_handle         lineColorVB     = {};    // color vertex buffer
            rz_gfx_buffer_handle         pointIB         = {};
            rz_gfx_buffer_handle         pointPosVB      = {};
            rz_gfx_buffer_handle         pointColorVB    = {};
            u32                          lineIndexCount  = 0;
            u32                          pointIndexCount = 0;
        };

        static DebugDrawState* s_pDebugDrawState = NULL;    // Singleton instance

        //---------------------------------------------------------------------------------------------------------------
        void RZDebugDraw::StartUp()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (s_pDebugDrawState != NULL) {
                RAZIX_CORE_WARN("RZDebugDraw::StartUp: Debug Draw already started!");
                return;
            }

            s_pDebugDrawState = (DebugDrawState*) rz_malloc(sizeof(DebugDrawState), 16);
            RAZIX_CORE_ASSERT(s_pDebugDrawState, "RZDebugDraw::StartUp: Failed to allocate memory for Debug Draw State!");
            new (s_pDebugDrawState) DebugDrawState();
            memset(s_pDebugDrawState, 0x00, sizeof(DebugDrawState));

            s_pDebugDrawState->drawList.m_DebugLines.clear();
            s_pDebugDrawState->drawList.m_DebugPoints.clear();
            s_pDebugDrawState->drawList.m_DebugLines.reserve(MaxLines);
            s_pDebugDrawState->drawList.m_DebugPoints.reserve(MaxPoints);
            s_pDebugDrawState->lineIndexCount  = 0;
            s_pDebugDrawState->pointIndexCount = 0;

            // load shaders
            s_pDebugDrawState->lineShader  = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::kDebugLine);
            s_pDebugDrawState->pointShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::kDebugPoint);

            // root sigs
            s_pDebugDrawState->lineRootSig  = RZResourceManager::Get().getShaderResource(s_pDebugDrawState->lineShader)->rootSignature;
            s_pDebugDrawState->pointRootSig = RZResourceManager::Get().getShaderResource(s_pDebugDrawState->pointShader)->rootSignature;

            // Build pipelines (follow ImGui pipeline style)
            rz_gfx_pipeline_desc pipelineDesc = {};
            // Line Pipeline
            pipelineDesc.cullMode               = RZ_GFX_CULL_MODE_TYPE_NONE;
            pipelineDesc.drawType               = RZ_GFX_DRAW_TYPE_LINE;
            pipelineDesc.polygonMode            = RZ_GFX_POLYGON_MODE_TYPE_SOLID;
            pipelineDesc.pShader                = RZResourceManager::Get().getShaderResource(s_pDebugDrawState->lineShader);
            pipelineDesc.pRootSig               = RZResourceManager::Get().getRootSignatureResource(s_pDebugDrawState->lineRootSig);
            pipelineDesc.type                   = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
            pipelineDesc.blendPreset            = RZ_GFX_BLEND_PRESET_ALPHA_BLEND;
            pipelineDesc.transparencyEnabled    = true;
            pipelineDesc.depthTestEnabled       = false;
            pipelineDesc.depthWriteEnabled      = false;
            pipelineDesc.depthCompareOp         = RZ_GFX_COMPARE_OP_TYPE_LESS_OR_EQUAL;
            pipelineDesc.blendEnabled           = true;
            pipelineDesc.renderTargetCount      = 1;
            pipelineDesc.renderTargetFormats[0] = RZ_GFX_FORMAT_R16G16B16A16_FLOAT;
            pipelineDesc.depthStencilFormat     = RZ_GFX_FORMAT_D16_UNORM;
            pipelineDesc.inputLayoutMode        = RZ_GFX_INPUT_LAYOUT_SOA;    // position @ 0 and color @ 1
            s_pDebugDrawState->linePipeline     = RZResourceManager::Get().createPipeline("Pipeline.DebugDraw::Line", pipelineDesc);

            // Point Pipeline
            pipelineDesc.drawType            = RZ_GFX_DRAW_TYPE_TRIANGLE;
            pipelineDesc.pShader             = RZResourceManager::Get().getShaderResource(s_pDebugDrawState->pointShader);
            pipelineDesc.pRootSig            = RZResourceManager::Get().getRootSignatureResource(s_pDebugDrawState->pointRootSig);
            s_pDebugDrawState->pointPipeline = RZResourceManager::Get().createPipeline("Pipeline.DebugDraw::Points", pipelineDesc);

            // Lines - position VB
            rz_gfx_buffer_desc vbDesc    = {};
            vbDesc.type                  = RZ_GFX_BUFFER_TYPE_VERTEX;
            vbDesc.resourceHints         = RZ_GFX_RESOURCE_VIEW_FLAG_SRV;
            vbDesc.usage                 = RZ_GFX_BUFFER_USAGE_TYPE_PERSISTENT_STREAM;
            vbDesc.sizeInBytes           = LinePositionVertexBufferSize;
            s_pDebugDrawState->linePosVB = RZResourceManager::Get().createBuffer("VB.Lines::Positions", vbDesc);

            // Lines - color VB
            vbDesc.sizeInBytes             = LineColorVertexBufferSize;
            s_pDebugDrawState->lineColorVB = RZResourceManager::Get().createBuffer("VB.Lines::Colors", vbDesc);

            // Lines - index buffer
            rz_gfx_buffer_desc ibDesc = {};
            ibDesc.type               = RZ_GFX_BUFFER_TYPE_INDEX;
            ibDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_SRV;
            ibDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_DYNAMIC;
            ibDesc.sizeInBytes        = MaxLineIndices * sizeof(u32);
            s_pDebugDrawState->lineIB = RZResourceManager::Get().createBuffer("IB.Lines", ibDesc);
            {
                // fill identity indices
                u32* line_indices = (u32*) rz_malloc(MaxLineIndices * sizeof(u32), 16);
                for (u32 i = 0; i < MaxLineIndices; ++i)
                    line_indices[i] = i;

                void* dst = rzRHI_MapBuffer(s_pDebugDrawState->lineIB, 0, MaxLineIndices * sizeof(u32));
                memcpy(dst, line_indices, MaxLineIndices * sizeof(u32));
                rzRHI_UnmapBuffer(s_pDebugDrawState->lineIB);

                rz_free(line_indices);
            }

            // Points - position VB
            vbDesc.sizeInBytes            = PointPositionVertexBufferSize;
            s_pDebugDrawState->pointPosVB = RZResourceManager::Get().createBuffer("VB.Points::Positions", vbDesc);
            // Points - color VB
            vbDesc.sizeInBytes              = PointColorVertexBufferSize;
            s_pDebugDrawState->pointColorVB = RZResourceManager::Get().createBuffer("VB.Points::Colors", vbDesc);

            // Points - index buffer
            ibDesc.sizeInBytes         = MaxPointIndices * sizeof(u32);
            s_pDebugDrawState->pointIB = RZResourceManager::Get().createBuffer("IB.Points", ibDesc);
            // Pre-fill point index buffer with quad indices (0,1,2,2,3,0) pattern
            {
                u32* indices = (u32*) rz_malloc(MaxPointIndices * sizeof(u32), 16);
                u32  offset  = 0;
                for (u32 i = 0; i < MaxPointIndices; i += 6) {
                    indices[i + 0] = offset + 0;
                    indices[i + 1] = offset + 1;
                    indices[i + 2] = offset + 2;

                    indices[i + 3] = offset + 2;
                    indices[i + 4] = offset + 3;
                    indices[i + 5] = offset + 0;

                    offset += 4;
                }
                void* dst = rzRHI_MapBuffer(s_pDebugDrawState->pointIB, 0, MaxPointIndices * sizeof(u32));
                memcpy(dst, indices, MaxPointIndices * sizeof(u32));
                rzRHI_UnmapBuffer(s_pDebugDrawState->pointIB);

                rz_free(indices);
            }
        }

        void RZDebugDraw::ShutDown()
        {
            RZResourceManager::Get().destroyPipeline(s_pDebugDrawState->linePipeline);
            RZResourceManager::Get().destroyPipeline(s_pDebugDrawState->pointPipeline);

            RZResourceManager::Get().destroyBuffer(s_pDebugDrawState->pointPosVB);
            RZResourceManager::Get().destroyBuffer(s_pDebugDrawState->pointColorVB);
            RZResourceManager::Get().destroyBuffer(s_pDebugDrawState->pointIB);

            RZResourceManager::Get().destroyBuffer(s_pDebugDrawState->linePosVB);
            RZResourceManager::Get().destroyBuffer(s_pDebugDrawState->lineColorVB);
            RZResourceManager::Get().destroyBuffer(s_pDebugDrawState->lineIB);

            if (s_pDebugDrawState != NULL) {
                rz_free(s_pDebugDrawState);
                s_pDebugDrawState = NULL;
            }
        }

        void RZDebugDraw::BeginDraw(const Razix::RZCamera3D* camera)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_CORE_ASSERT(camera != NULL, "Debug Draw::BeginDraw: Camera is null!");

            s_pDebugDrawState->lineIndexCount  = 0;
            s_pDebugDrawState->pointIndexCount = 0;

            // FIXME: when using VMA, once enabled will skip MapBuffer calls and return the cached mapped pointer
            // same goes for DXMA as well, until then we need to unmap before mapping again, this is temporary fix

            // LINES
            {
                void* posPtr = rzRHI_MapBuffer(s_pDebugDrawState->linePosVB, 0, LinePositionVertexBufferSize);
                void* colPtr = rzRHI_MapBuffer(s_pDebugDrawState->lineColorVB, 0, LineColorVertexBufferSize);

                float4* linePositionData = static_cast<float4*>(posPtr);
                float4* lineColorData    = static_cast<float4*>(colPtr);

                u32 lineIdx = 0;
                for (auto& line: s_pDebugDrawState->drawList.m_DebugLines) {
                    linePositionData[lineIdx] = float4(line.p1, 1.0f);
                    lineColorData[lineIdx]    = line.col;
                    lineIdx++;

                    linePositionData[lineIdx] = float4(line.p2, 1.0f);
                    lineColorData[lineIdx]    = line.col;
                    lineIdx++;

                    s_pDebugDrawState->lineIndexCount += 2;
                }

                rzRHI_UnmapBuffer(s_pDebugDrawState->linePosVB);
                rzRHI_UnmapBuffer(s_pDebugDrawState->lineColorVB);
            }

            // POINTS
            {
                // Early out if no camera
                if (!camera) return;

                void* vtxPtr = rzRHI_MapBuffer(s_pDebugDrawState->pointPosVB, 0, PointPositionVertexBufferSize);
                void* colPtr = rzRHI_MapBuffer(s_pDebugDrawState->pointColorVB, 0, PointColorVertexBufferSize);

                float4* pointsVertexData = (float4*) vtxPtr;
                float4* pointsColorData  = (float4*) colPtr;

                u32 pointIdx = 0;
                for (auto& point: s_pDebugDrawState->drawList.m_DebugPoints) {
                    float3 right = point.size * camera->getRight();
                    float3 up    = point.size * camera->getUp();

                    // Define the four corners of the quad
                    float4 quadPositions[4] = {
                        float4(point.p1 - right - up, 1.0f),    // Bottom-left
                        float4(point.p1 + right - up, 1.0f),    // Bottom-right
                        float4(point.p1 + right + up, 1.0f),    // Top-right
                        float4(point.p1 - right + up, 1.0f)     // Top-left
                    };

                    for (int i = 0; i < 4; i++) {
                        pointsVertexData[pointIdx] = quadPositions[i];
                        pointsColorData[pointIdx]  = point.col;
                        pointIdx++;
                    }

                    s_pDebugDrawState->pointIndexCount += 6;    // For quad rendering
                }

                rzRHI_UnmapBuffer(s_pDebugDrawState->pointPosVB);
                rzRHI_UnmapBuffer(s_pDebugDrawState->pointColorVB);
            }
        }

        void RZDebugDraw::IssueDrawCommands(rz_gfx_cmdbuf_handle cmdBuffer, rz_gfx_descriptor_heap_handle heap, rz_gfx_descriptor_table_handle frameData)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            rz_gfx_descriptor_heap_handle heaps[] = {
                heap    // FrameData
            };
            rzRHI_BindDescriptorHeaps(cmdBuffer, heaps, 1);
            // Bind frame data descriptor table
            rz_gfx_descriptor_table_handle tables[] = {
                frameData,
            };

            // Lines
            if (s_pDebugDrawState->lineIndexCount > 0) {
                rzRHI_BindGfxRootSig(cmdBuffer, s_pDebugDrawState->lineRootSig);
                rzRHI_BindPipeline(cmdBuffer, s_pDebugDrawState->linePipeline);
                rzRHI_BindDescriptorTables(cmdBuffer, RZ_GFX_PIPELINE_TYPE_GRAPHICS, s_pDebugDrawState->lineRootSig, tables, 1);

                u32                  offsetsP[2] = {0, 0};
                u32                  stridesP[2] = {sizeof(float4), sizeof(float4)};
                rz_gfx_buffer_handle vbsP[2]     = {s_pDebugDrawState->linePosVB, s_pDebugDrawState->lineColorVB};
                rzRHI_BindVertexBuffers(cmdBuffer, vbsP, 2, offsetsP, stridesP);
                rzRHI_BindIndexBuffer(cmdBuffer, s_pDebugDrawState->lineIB, 0, RZ_GFX_INDEX_TYPE_UINT32);

                rzRHI_DrawIndexedAuto(cmdBuffer, s_pDebugDrawState->lineIndexCount, 1, 0, 0, 0);
            }

            // Points
            if (s_pDebugDrawState->pointIndexCount > 0) {
                rzRHI_BindGfxRootSig(cmdBuffer, s_pDebugDrawState->pointRootSig);
                rzRHI_BindPipeline(cmdBuffer, s_pDebugDrawState->pointPipeline);
                rzRHI_BindDescriptorTables(cmdBuffer, RZ_GFX_PIPELINE_TYPE_GRAPHICS, s_pDebugDrawState->pointRootSig, tables, 1);

                u32                  offsetsP[2] = {0, 0};
                u32                  stridesP[2] = {sizeof(float4), sizeof(float4)};
                rz_gfx_buffer_handle vbsP[2]     = {s_pDebugDrawState->pointPosVB, s_pDebugDrawState->pointColorVB};
                rzRHI_BindVertexBuffers(cmdBuffer, vbsP, 2, offsetsP, stridesP);
                rzRHI_BindIndexBuffer(cmdBuffer, s_pDebugDrawState->pointIB, 0, RZ_GFX_INDEX_TYPE_UINT32);

                rzRHI_DrawIndexedAuto(cmdBuffer, s_pDebugDrawState->pointIndexCount, 1, 0, 0, 0);
            }
        }

        void RZDebugDraw::EndDraw()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            s_pDebugDrawState->drawList.m_DebugPoints.clear();
            s_pDebugDrawState->drawList.m_DebugLines.clear();
            s_pDebugDrawState->lineIndexCount  = 0;
            s_pDebugDrawState->pointIndexCount = 0;
        }

        //---------------------------------------------------------------------------------------------------------------

        void PopulateLinesDrawList(bool dt, const float3& start, const float3& end, const float4& colour)
        {
            RAZIX_CORE_ASSERT(s_pDebugDrawState != NULL, "Debug Draw State not initialized!");

            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (s_pDebugDrawState)
                s_pDebugDrawState->drawList.m_DebugLines.emplace_back(start, end, colour);
        }

        void PopulatePointsDrawList(bool dt, const float3& pos, f32 point_radius, const float4& colour)
        {
            RAZIX_CORE_ASSERT(s_pDebugDrawState != NULL, "Debug Draw State not initialized!");

            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (s_pDebugDrawState)
                s_pDebugDrawState->drawList.m_DebugPoints.emplace_back(pos, point_radius, colour);
        }

        //---------------------------------------------------------------------------------------------------------------

        void RZDebugDraw::DrawPoint(const float3& pos, f32 point_radius, const float3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulatePointsDrawList(false, pos, point_radius, float4(colour, 1.0f));
        }
        void RZDebugDraw::DrawPoint(const float3& pos, f32 point_radius, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulatePointsDrawList(false, pos, point_radius, colour);
        }
        void RZDebugDraw::DrawLine(const float3& start, const float3& end, const float3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateLinesDrawList(false, start, end, float4(colour, 1.0f));
        }
        void RZDebugDraw::DrawLine(const float3& start, const float3& end, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateLinesDrawList(false, start, end, colour);
        }

        void RZDebugDraw::DrawAABB(const Maths::AABB& box, const float4& edgeColour, bool cornersOnly /*= false*/, f32 width /*= 0.02f*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // 8 vertices
            float3 v1(box.min.x, box.min.y, box.min.z);
            float3 v2(box.min.x, box.min.y, box.max.z);
            float3 v3(box.min.x, box.max.y, box.min.z);
            float3 v4(box.min.x, box.max.y, box.max.z);
            float3 v5(box.max.x, box.min.y, box.min.z);
            float3 v6(box.max.x, box.min.y, box.max.z);
            float3 v7(box.max.x, box.max.y, box.min.z);
            float3 v8(box.max.x, box.max.y, box.max.z);

            // 12 edges
            DrawLine(v1, v2, edgeColour);
            DrawLine(v1, v3, edgeColour);
            DrawLine(v1, v5, edgeColour);
            DrawLine(v2, v4, edgeColour);
            DrawLine(v2, v6, edgeColour);
            DrawLine(v3, v4, edgeColour);
            DrawLine(v3, v7, edgeColour);
            DrawLine(v4, v8, edgeColour);
            DrawLine(v5, v6, edgeColour);
            DrawLine(v5, v7, edgeColour);
            DrawLine(v6, v8, edgeColour);
            DrawLine(v7, v8, edgeColour);
        }

        void RZDebugDraw::DrawGrid(u32 dimension, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            float pos = float(dimension) / 2.0f;
            for (float x = -(float) dimension / 2.0f; x <= (float) dimension / 2.0f; x++) {
                DrawLine(float3(x, 0, -pos), float3(x, 0, pos), colour);
                DrawLine(float3(-pos, 0, x), float3(pos, 0, x), colour);
            }
        }

        void RZDebugDraw::DrawLight(Gfx::RZLight* light, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            RAZIX_UNUSED(light);
            RAZIX_UNUSED(colour);

            // Directional
            //if (light->getType() == RZ_LIGHT_TYPE_DIRECTIONAL) {
            //    float3 offset(0.0f, 0.1f, 0.0f);
            //    auto   lightPos = normalize(-light->getPosition());
            //    DrawLine(float3(light->getPosition()) + offset, float3(lightPos * 2.0f) + offset, colour);
            //    DrawLine(float3(light->getPosition()) - offset, float3(lightPos * 2.0f) - offset, colour);
            //    DrawLine(float3(light->getPosition()), float3(lightPos * 2.0f), colour);
            //    //DrawCone(20, 4, 30.0f, 1.5f, (light->getPosition() - (light->getDirection()) * 1.5f), rotation, colour);
            //}
            ////// Spot
            ////else if (light->Type < 1.1f) {
            ////    DrawCone(20, 4, light->getAngle(), light->getIntensity(), light->getPosition(), rotation, colour);
            ////}
            //else if (light->getType() == RZ_LIGHT_TYPE_POINT) {
            //    DrawSphere(light->getRadius() * 0.5f, light->getPosition(), colour);
            //}
        }

        void RZDebugDraw::DrawFrustum(const Maths::RZFrustum& frustum, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            const auto* vertices = frustum.getVertices();

            RZDebugDraw::DrawLine(vertices[0], vertices[1], colour);
            RZDebugDraw::DrawLine(vertices[1], vertices[2], colour);
            RZDebugDraw::DrawLine(vertices[2], vertices[3], colour);
            RZDebugDraw::DrawLine(vertices[3], vertices[0], colour);
            RZDebugDraw::DrawLine(vertices[4], vertices[5], colour);
            RZDebugDraw::DrawLine(vertices[5], vertices[6], colour);
            RZDebugDraw::DrawLine(vertices[6], vertices[7], colour);
            RZDebugDraw::DrawLine(vertices[7], vertices[4], colour);
            RZDebugDraw::DrawLine(vertices[0], vertices[4], colour);
            RZDebugDraw::DrawLine(vertices[1], vertices[5], colour);
            RZDebugDraw::DrawLine(vertices[2], vertices[6], colour);
            RZDebugDraw::DrawLine(vertices[3], vertices[7], colour);
        }

        void RZDebugDraw::DrawFrustum(const float4x4& mat, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto frustum = Maths::RZFrustum();
            frustum.build(mat);

            const auto* vertices = frustum.getVertices();

            RZDebugDraw::DrawLine(vertices[0], vertices[1], colour);
            RZDebugDraw::DrawLine(vertices[1], vertices[2], colour);
            RZDebugDraw::DrawLine(vertices[2], vertices[3], colour);
            RZDebugDraw::DrawLine(vertices[3], vertices[0], colour);
            RZDebugDraw::DrawLine(vertices[4], vertices[5], colour);
            RZDebugDraw::DrawLine(vertices[5], vertices[6], colour);
            RZDebugDraw::DrawLine(vertices[6], vertices[7], colour);
            RZDebugDraw::DrawLine(vertices[7], vertices[4], colour);
            RZDebugDraw::DrawLine(vertices[0], vertices[4], colour);
            RZDebugDraw::DrawLine(vertices[1], vertices[5], colour);
            RZDebugDraw::DrawLine(vertices[2], vertices[6], colour);
            RZDebugDraw::DrawLine(vertices[3], vertices[7], colour);
        }

        void RZDebugDraw::DrawCircle(int numVerts, f32 radius, const float3& position, const float3& eulerRotation, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            f32 sectorAngle = 360.0f / f32(numVerts);

            quat rotation = quat(float3(radians(eulerRotation.x), radians(eulerRotation.y), radians(eulerRotation.z)));

            for (f32 angle = 0; angle <= 360.0f; angle += sectorAngle) {
                f32    cx      = cos(radians(angle)) * radius;
                f32    cy      = sin(radians(angle)) * radius;
                float3 current = float3(cx, cy, 0.0f);

                f32    nx   = cos(radians(angle + sectorAngle)) * radius;
                f32    ny   = sin(radians(angle + sectorAngle)) * radius;
                float3 next = float3(nx, ny, 0.0f);

                //DrawPoint(position + (current), 0.05, colour);
                DrawLine(position + (rotation * current), position + (rotation * next), colour);
            }
        }

        void RZDebugDraw::DrawCylinder(const float3& position, const float3& eulerRotation, float height, float radius, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            quat rotation = quat(float3(radians(eulerRotation.x), radians(eulerRotation.y), radians(eulerRotation.z)));

            float3 up = (rotation * float3(0.0f, 1.0f, 0.0f));

            float3 topSphereCentre    = position + up * (height * 0.5f);
            float3 bottomSphereCentre = position - up * (height * 0.5f);

            DrawCircle(20, radius, topSphereCentre, eulerRotation + float3(90.0f, 0.0f, 0.0f), colour);
            DrawCircle(20, radius, bottomSphereCentre, eulerRotation + float3(90.0f, 0.0f, 0.0f), colour);

            // Draw 10 arcs
            // Sides
            float step = 360.0f / float(20);
            for (int i = 0; i < 20; i++) {
                float z = cos(step * i) * radius;
                float x = sin(step * i) * radius;

                float3 offset = rotation * float4(x, 0.0f, z, 0.0f);
                DrawLine(bottomSphereCentre + offset, topSphereCentre + offset, colour);
            }
        }

        void RZDebugDraw::DrawCapsule(const float3& position, const float3& eulerRotation, float height, float radius, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            quat rotation = quat(float3(radians(eulerRotation.x), radians(eulerRotation.y), radians(eulerRotation.z)));

            float3 up = (rotation * float3(0.0f, 1.0f, 0.0f));

            float3 topSphereCentre    = position + up * (height * 0.5f);
            float3 bottomSphereCentre = position - up * (height * 0.5f);

            DrawCircle(20, radius, topSphereCentre, eulerRotation + float3(90.0f, 0.0f, 0.0f), colour);
            DrawCircle(20, radius, bottomSphereCentre, eulerRotation + float3(90.0f, 0.0f, 0.0f), colour);

            // Draw 10 arcs
            // Sides
            f32 sectorAngle = 360.0f / f32(10.0f);
            for (f32 angle = 0; angle <= 360.0f; angle += sectorAngle) {
                f32 x = cos(radians(angle)) * radius;
                f32 z = sin(radians(angle)) * radius;

                float3 offset = rotation * float4(x, 0.0f, z, 0.0f);
                DrawLine(bottomSphereCentre + offset, topSphereCentre + offset, colour);

                if (angle <= 180.0f) {
                    float z2 = cos(radians(angle + 180.0f)) * radius;
                    float x2 = sin(radians(angle + 180.0f)) * radius;

                    float3 offset2 = rotation * float4(x2, 0.0f, z2, 0.0f);
                    // Top HemiShpere
                    DrawArc(20, radius, topSphereCentre + offset, topSphereCentre + offset2, eulerRotation, colour);
                    DrawPoint(topSphereCentre + offset2, 0.1f, float4(0.0f, 0.0f, 0.0f, 1.0f));
                    // Bottom Hemisphere
                    //DebugDrawArc(20, radius, bottomSphereCentre + offset, bottomSphereCentre + offset, eulerRotation + float3(180.0f, 0.0f, 0.0f), colour);
                    DrawPoint(bottomSphereCentre + offset2, 0.1f, float4(0.0f, 0.0f, 0.0f, 1.0f));
                }
            }
        }

        void RZDebugDraw::DrawArc(int numVerts, float radius, const float3& start, const float3& end, const float3& eulerRotation, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            f32 sectorAngle = 180.0f / f32(numVerts);

            quat rotation = quat(float3(radians(eulerRotation.x), radians(eulerRotation.y), radians(eulerRotation.z)));

            float3 arcCentre = (start + end) * 0.5f;
            arcCentre        = arcCentre - (radius * 0.5f);
            for (f32 angle = 0; angle <= 180.0f; angle += sectorAngle) {
                f32    cx      = cos(radians(angle)) * radius;
                f32    cy      = sin(radians(angle)) * radius;
                float3 current = float3(cx, cy, 0.0f);

                f32    nx   = cos(radians(angle + sectorAngle)) * radius;
                f32    ny   = sin(radians(angle + sectorAngle)) * radius;
                float3 next = float3(nx, ny, 0.0f);

                //DrawPoint(position + (current), 0.05, colour);
                DrawLine(arcCentre + (rotation * current), arcCentre + (rotation * next), colour);
                DrawPoint(arcCentre + (rotation * current), 0.1f, float4(0.0f, 0.0f, 0.0f, 1.0f));
            }
        }

        void RZDebugDraw::DrawSphere(f32 radius, const float3& position, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            DrawCircle(50, radius, position, float3(90.0f, 0.0f, 0.0f), colour);
            DrawCircle(50, radius, position, float3(45.0f, 0.0f, 0.0f), colour);
            DrawCircle(50, radius, position, float3(-45.0f, 0.0f, 0.0f), colour);
            DrawCircle(50, radius, position, float3(0.0f, 90.0f, 0.0f), colour);
            DrawCircle(50, radius, position, float3(0.0f, 45.0f, 0.0f), colour);
            DrawCircle(50, radius, position, float3(00.0f, -45.0f, 0.0f), colour);
            DrawCircle(50, radius, position, float3(0.0f, 0.0f, 90.0f), colour);
            DrawCircle(50, radius, position, float3(0.0f, 0.0f, 45.0f), colour);
            DrawCircle(50, radius, position, float3(0.0f, 0.0f, -45.0f), colour);
        }

        void RZDebugDraw::DrawCone(int numCircleVerts, int numLinesToCircle, f32 angle, f32 length, const float3& position, const float3& rotation, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            f32  radius       = tan(radians(angle * 0.5f)) * length;
            quat quatRotation = quat(float3(radians(rotation.x), radians(rotation.y), radians(rotation.z)));

            float3 forward     = -(quatRotation * float3(0.0f, 0.0f, -1.0f));
            float3 endPosition = position + forward * length;
            DrawCircle(numCircleVerts, radius, endPosition, rotation, colour);

            // FIXME: Use the draw circle logic and get the points on the circle and draw lines to it from the origin
            f32 sectorAngle = 360.0f / f32(numLinesToCircle);
            for (f32 sec_angle = 0; sec_angle <= 360.0f; sec_angle += sectorAngle) {
                f32 cx = cos(radians(sec_angle)) * radius;
                f32 cy = sin(radians(sec_angle)) * radius;

                float3 point    = float3(cx, cy, 0.0f);
                float3 endPoint = endPosition + point;
                DrawLine(position, endPoint, colour);
                //DrawPoint(endPoint,0.1f, colour);
            }
        }
    }    // namespace Gfx
}    // namespace Razix
