// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZDebugDraw.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/Lighting/RZLight.h"
#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {

        struct DebugLine
        {
            float3 p1;
            float  _pad0;
            float3 p2;
            float  _pad1;
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
            RZDynamicArray<DebugLine>  m_DebugLines;
            RZDynamicArray<DebugPoint> m_DebugPoints;
        };

        struct DebugDrawState
        {
            DebugDrawList          drawList;
            rz_gfx_pipeline_handle linePipeline    = {};
            rz_gfx_pipeline_handle pointPipeline   = {};
            rz_gfx_shader_handle   lineShader      = {};
            rz_gfx_shader_handle   pointShader     = {};
            rz_gfx_buffer_handle   lineIB          = {};    // index buffer
            rz_gfx_buffer_handle   linePosVB       = {};    // position vertex buffer
            rz_gfx_buffer_handle   lineColorVB     = {};    // color vertex buffer
            rz_gfx_buffer_handle   pointIB         = {};
            rz_gfx_buffer_handle   pointPosVB      = {};
            rz_gfx_buffer_handle   pointColorVB    = {};
            u32                    lineIndexCount  = 0;
            u32                    pointIndexCount = 0;
        };

        static DebugDrawState s_DebugDrawState;

        //---------------------------------------------------------------------------------------------------------------
        void RZDebugDraw::Init()
        {
            m_PointShader                       = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::DebugPoint);
            Gfx::RZPipelineDesc pipelineInfo    = {};
            pipelineInfo.name                   = "Pipeline.DebugRender::Points(DT)";
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.shader                 = m_PointShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Gfx::TextureFormat::RGBA16F};
            pipelineInfo.depthFormat            = Gfx::TextureFormat::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            pipelineInfo.depthOp                = CompareOp::LessOrEqual;
            // Points Pipeline
            m_PointPipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            // Change the polygon mode for drawing lines
            m_LineShader             = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::DebugLine);
            pipelineInfo.name        = "Pipeline.DebugRenderer::Lines(DT)";
            pipelineInfo.shader      = m_LineShader;
            pipelineInfo.cullMode    = CullMode::None;
            pipelineInfo.polygonMode = PolygonMode::Fill;
            pipelineInfo.drawType    = DrawType::Line;
            // Line Pipeline
            m_LinePipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            RZBufferDesc pointVBDesc          = {};
            pointVBDesc.data                  = nullptr;
            pointVBDesc.usage                 = BufferUsage::PersistentStream;
            pointVBDesc.initResourceViewHints = kCBV;

            pointVBDesc.name   = "VB_Points::Positions";
            pointVBDesc.size   = PointPositionVertexBufferSize;
            m_PointPosition_VB = RZResourceManager::Get().createVertexBuffer(pointVBDesc);

            pointVBDesc.name = "VB_Points::Colors";
            pointVBDesc.size = PointColorVertexBufferSize;
            m_PointColor_VB  = RZResourceManager::Get().createVertexBuffer(pointVBDesc);

            //----------------------------------------

            u32* indices = (u32*) Razix::Memory::RZMalloc(MaxPointIndices * sizeof(u32));
            u32  offset  = 0;
            for (u32 i = 0; i < MaxPointIndices; i += 6) {
                indices[i]     = offset + 0;
                indices[i + 1] = offset + 1;
                indices[i + 2] = offset + 2;

                indices[i + 3] = offset + 2;
                indices[i + 4] = offset + 3;
                indices[i + 5] = offset + 0;

                offset += 4;
            }

            RZBufferDesc pointIBDesc          = {};
            pointIBDesc.name                  = "IB_Points";
            pointIBDesc.usage                 = BufferUsage::Static;
            pointIBDesc.count                 = MaxPointIndices;
            pointIBDesc.data                  = indices;
            pointIBDesc.initResourceViewHints = ResourceViewHint::kCBV;
            m_PointIB                         = RZResourceManager::Get().createIndexBuffer(pointIBDesc);

            Razix::Memory::RZFree(indices);

            RZBufferDesc lineVBDesc          = {};
            lineVBDesc.usage                 = BufferUsage::PersistentStream;
            lineVBDesc.data                  = nullptr;
            lineVBDesc.initResourceViewHints = kCBV;

            lineVBDesc.name   = "VB_Lines::Positions";
            lineVBDesc.size   = LinePositionVertexBufferSize;
            m_LinePosition_VB = RZResourceManager::Get().createVertexBuffer(lineVBDesc);

            lineVBDesc.name = "VB_Lines::Colors";
            lineVBDesc.size = LineColorVertexBufferSize;
            m_LineColor_VB  = RZResourceManager::Get().createVertexBuffer(lineVBDesc);

            //----------------------------------------

            u32* line_indices = (u32*) Razix::Memory::RZMalloc(MaxLineIndices * sizeof(u32));

            for (uint32_t i = 0; i < MaxLineIndices; i++) {
                line_indices[i] = i;
            }

            RZBufferDesc lineIBDesc          = {};
            lineIBDesc.name                  = "IB_Lines";
            lineIBDesc.usage                 = BufferUsage::Static;
            lineIBDesc.count                 = MaxLineIndices;
            lineIBDesc.data                  = line_indices;
            lineIBDesc.initResourceViewHints = kCBV;
            m_LineIB                         = RZResourceManager::Get().createIndexBuffer(lineIBDesc);

            Memory::RZFree(line_indices);
        }

        void RZDebugDraw::Begin(RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            auto& sceneCamera = scene->getSceneCamera();

            RAZIX_MARK_BEGIN("Debug Renderer Pass", float4(0.0f, 0.85f, 0.0f, 1.0f));

            // POINTS
            {
                // Prepare the points VBO and IBO and update them

                auto pointPositionVBResource = RZResourceManager::Get().getVertexBufferResource(m_PointPosition_VB);
                auto pointColorVBResource    = RZResourceManager::Get().getVertexBufferResource(m_PointColor_VB);

                // Map the VBO
                pointPositionVBResource->Map(PointPositionVertexBufferSize);
                float3* pointsVertexData = (float3*) pointPositionVBResource->GetMappedBuffer();

                pointColorVBResource->Map(PointColorVertexBufferSize);
                float3* pointsColorData = (float3*) pointColorVBResource->GetMappedBuffer();

                u32 pointIdx = 0;
                for (auto& point: m_DrawList.m_DebugPoints) {
                    float3 right = point.size * sceneCamera.getRight();
                    float3 up    = point.size * sceneCamera.getUp();

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

                    m_PointIndexCount += 6;    // For quad rendering
                }

                pointPositionVBResource->Flush();
                pointColorVBResource->Flush();

                auto pointIBResource = RZResourceManager::Get().getIndexBufferResource(m_PointIB);
                pointIBResource->setCount(m_PointIndexCount);
            }

            // LINES
            {
                auto linePositionVBResource = RZResourceManager::Get().getVertexBufferResource(m_LinePosition_VB);
                auto lineColorVBResource    = RZResourceManager::Get().getVertexBufferResource(m_LineColor_VB);

                // Map the VBOs
                linePositionVBResource->Map(LinePositionVertexBufferSize);
                float4* linePositionData = static_cast<float4*>(linePositionVBResource->GetMappedBuffer());

                lineColorVBResource->Map(LineColorVertexBufferSize);
                float4* lineColorData = static_cast<float4*>(lineColorVBResource->GetMappedBuffer());

                u32 lineIdx = 0;
                for (auto& line: m_DrawList.m_DebugLines) {
                    linePositionData[lineIdx] = float4(line.p1, 1.0f);
                    lineColorData[lineIdx]    = line.col;
                    lineIdx++;

                    linePositionData[lineIdx] = float4(line.p2, 1.0f);
                    lineColorData[lineIdx]    = line.col;
                    lineIdx++;

                    m_LineIndexCount += 2;
                }

                linePositionVBResource->Flush();
                lineColorVBResource->Flush();

                auto lineIBResource = RZResourceManager::Get().getIndexBufferResource(m_LineIB);
                lineIBResource->setCount(m_LineIndexCount);
            }
        }

        void RZDebugDraw::Draw(RZDrawCommandBufferHandle cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Points
            {
                RHI::BindPipeline(m_PointPipeline, cmdBuffer);

                Gfx::RHI::BindDescriptorSet(m_PointPipeline, cmdBuffer, RHI::Get().getFrameDataSet(), BindingTable_System::SET_IDX_FRAME_DATA);

                auto pointPositionVBResource = RZResourceManager::Get().getVertexBufferResource(m_PointPosition_VB);
                auto pointColorVBResource    = RZResourceManager::Get().getVertexBufferResource(m_PointColor_VB);

                auto pointIBResource = RZResourceManager::Get().getIndexBufferResource(m_PointIB);

                pointPositionVBResource->Bind(cmdBuffer, 0);
                pointColorVBResource->Bind(cmdBuffer, 1);
                pointIBResource->Bind(cmdBuffer);

                RHI::DrawIndexed(cmdBuffer, m_PointIndexCount);
            }

            // Lines
            {
                RHI::BindPipeline(m_LinePipeline, cmdBuffer);

                Gfx::RHI::BindDescriptorSet(m_LinePipeline, cmdBuffer, RHI::Get().getFrameDataSet(), BindingTable_System::SET_IDX_FRAME_DATA);

                auto linePositionVBResource = RZResourceManager::Get().getVertexBufferResource(m_LinePosition_VB);
                auto lineColorVBResource    = RZResourceManager::Get().getVertexBufferResource(m_LineColor_VB);
                auto lineIBResource         = RZResourceManager::Get().getIndexBufferResource(m_LineIB);

                linePositionVBResource->Bind(cmdBuffer, 0);
                lineColorVBResource->Bind(cmdBuffer, 1);
                lineIBResource->Bind(cmdBuffer);

                RHI::DrawIndexed(cmdBuffer, m_LineIndexCount);
            }
        }

        void RZDebugDraw::EndDraw()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_DrawList.m_DebugPoints.clear();
            m_DrawList.m_DebugLines.clear();
            m_DrawList.m_DebugThickLines.clear();
            m_PointIndexCount = 0;
            m_LineIndexCount  = 0;
        }

        void RZDebugDraw::Destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_LinePipeline);
            RZResourceManager::Get().destroyPipeline(m_PointPipeline);

            RZResourceManager::Get().destroyIndexBuffer(m_LineIB);
            RZResourceManager::Get().destroyVertexBuffer(m_LinePosition_VB);
            RZResourceManager::Get().destroyVertexBuffer(m_LineColor_VB);

            RZResourceManager::Get().destroyIndexBuffer(m_PointIB);
            RZResourceManager::Get().destroyVertexBuffer(m_PointPosition_VB);
            RZResourceManager::Get().destroyVertexBuffer(m_PointColor_VB);
        }

        //---------------------------------------------------------------------------------------------------------------

        void RZDebugDraw::createPipelines()
        {
            m_PointShader                       = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::DebugPoint);
            Gfx::RZPipelineDesc pipelineInfo    = {};
            pipelineInfo.name                   = "Pipeline.DebugRender::Points(DT)";
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.shader                 = m_PointShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Gfx::TextureFormat::RGBA16F};
            pipelineInfo.depthFormat            = Gfx::TextureFormat::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            pipelineInfo.depthOp                = CompareOp::LessOrEqual;
            // Points Pipeline
            m_PointPipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            // Change the polygon mode for drawing lines
            m_LineShader             = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::DebugLine);
            pipelineInfo.name        = "Pipeline.DebugRenderer::Lines(DT)";
            pipelineInfo.shader      = m_LineShader;
            pipelineInfo.cullMode    = CullMode::None;
            pipelineInfo.polygonMode = PolygonMode::Fill;
            pipelineInfo.drawType    = DrawType::Line;
            // Line Pipeline
            m_LinePipeline = RZResourceManager::Get().createPipeline(pipelineInfo);
        }

        void RZDebugDraw::createPointBufferResources()
        {
            RZBufferDesc pointVBDesc          = {};
            pointVBDesc.data                  = nullptr;
            pointVBDesc.usage                 = BufferUsage::PersistentStream;
            pointVBDesc.initResourceViewHints = kCBV;

            pointVBDesc.name   = "VB_Points::Positions";
            pointVBDesc.size   = PointPositionVertexBufferSize;
            m_PointPosition_VB = RZResourceManager::Get().createVertexBuffer(pointVBDesc);

            pointVBDesc.name = "VB_Points::Colors";
            pointVBDesc.size = PointColorVertexBufferSize;
            m_PointColor_VB  = RZResourceManager::Get().createVertexBuffer(pointVBDesc);

            //----------------------------------------

            u32* indices = (u32*) Razix::Memory::RZMalloc(MaxPointIndices * sizeof(u32));
            u32  offset  = 0;
            for (u32 i = 0; i < MaxPointIndices; i += 6) {
                indices[i]     = offset + 0;
                indices[i + 1] = offset + 1;
                indices[i + 2] = offset + 2;

                indices[i + 3] = offset + 2;
                indices[i + 4] = offset + 3;
                indices[i + 5] = offset + 0;

                offset += 4;
            }

            RZBufferDesc pointIBDesc          = {};
            pointIBDesc.name                  = "IB_Points";
            pointIBDesc.usage                 = BufferUsage::Static;
            pointIBDesc.count                 = MaxPointIndices;
            pointIBDesc.data                  = indices;
            pointIBDesc.initResourceViewHints = ResourceViewHint::kCBV;
            m_PointIB                         = RZResourceManager::Get().createIndexBuffer(pointIBDesc);

            Razix::Memory::RZFree(indices);
        }

        void RZDebugDraw::createLineBufferResources()
        {
            RZBufferDesc lineVBDesc          = {};
            lineVBDesc.usage                 = BufferUsage::PersistentStream;
            lineVBDesc.data                  = nullptr;
            lineVBDesc.initResourceViewHints = kCBV;

            lineVBDesc.name   = "VB_Lines::Positions";
            lineVBDesc.size   = LinePositionVertexBufferSize;
            m_LinePosition_VB = RZResourceManager::Get().createVertexBuffer(lineVBDesc);

            lineVBDesc.name = "VB_Lines::Colors";
            lineVBDesc.size = LineColorVertexBufferSize;
            m_LineColor_VB  = RZResourceManager::Get().createVertexBuffer(lineVBDesc);

            //----------------------------------------

            u32* line_indices = (u32*) Razix::Memory::RZMalloc(MaxLineIndices * sizeof(u32));

            for (uint32_t i = 0; i < MaxLineIndices; i++) {
                line_indices[i] = i;
            }

            RZBufferDesc lineIBDesc          = {};
            lineIBDesc.name                  = "IB_Lines";
            lineIBDesc.usage                 = BufferUsage::Static;
            lineIBDesc.count                 = MaxLineIndices;
            lineIBDesc.data                  = line_indices;
            lineIBDesc.initResourceViewHints = kCBV;
            m_LineIB                         = RZResourceManager::Get().createIndexBuffer(lineIBDesc);

            Memory::RZFree(line_indices);
        }

        //---------------------------------------------------------------------------------------------------------------
        // Public API

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
        void RZDebugDraw::DrawPointDT(const float3& pos, f32 point_radius, const float3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulatePointsDrawList(true, pos, point_radius, float4(colour, 1.0f));
        }
        void RZDebugDraw::DrawPointDT(const float3& pos, f32 point_radius, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulatePointsDrawList(true, pos, point_radius, colour);
        }

        void RZDebugDraw::DrawThickLine(const float3& start, const float3& end, f32 line_width, const float3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateThickLinesDrawList(false, start, end, line_width, float4(colour, 1.0f));
        }
        void RZDebugDraw::DrawThickLine(const float3& start, const float3& end, f32 line_width, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateThickLinesDrawList(false, start, end, line_width, colour);
        }
        void RZDebugDraw::DrawThickLineDT(const float3& start, const float3& end, f32 line_width, const float3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateThickLinesDrawList(true, start, end, line_width, float4(colour, 1.0f));
        }
        void RZDebugDraw::DrawThickLineDT(const float3& start, const float3& end, f32 line_width, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateThickLinesDrawList(true, start, end, line_width, colour);
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
        void RZDebugDraw::DrawLineDT(const float3& start, const float3& end, const float3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateLinesDrawList(true, start, end, float4(colour, 1.0f));
        }
        void RZDebugDraw::DrawLineDT(const float3& start, const float3& end, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateLinesDrawList(true, start, end, colour);
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

            // Directional
            if (light->getType() == LightType::DIRECTIONAL) {
                float3 offset(0.0f, 0.1f, 0.0f);
                auto   lightPos = normalize(-light->getPosition());
                DrawLine(float3(light->getPosition()) + offset, float3(lightPos * 2.0f) + offset, colour);
                DrawLine(float3(light->getPosition()) - offset, float3(lightPos * 2.0f) - offset, colour);
                DrawLine(float3(light->getPosition()), float3(lightPos * 2.0f), colour);
                //DrawCone(20, 4, 30.0f, 1.5f, (light->getPosition() - (light->getDirection()) * 1.5f), rotation, colour);
            }
            //// Spot
            //else if (light->Type < 1.1f) {
            //    DrawCone(20, 4, light->getAngle(), light->getIntensity(), light->getPosition(), rotation, colour);
            //}
            else if (light->getType() == LightType::Point) {
                DrawSphere(light->getRadius() * 0.5f, light->getPosition(), colour);
            }
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

        //---------------------------------------------------------------------------------------------------------------
        // Populate DrawList functions
        void RZDebugDraw::PopulateLinesDrawList(bool dt, const float3& start, const float3& end, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (dt)
                RZDebugDraw::Get().m_DrawListNDT.m_DebugLines.emplace_back(start, end, colour);
            else
                RZDebugDraw::Get().m_DrawList.m_DebugLines.emplace_back(start, end, colour);
        }

        void RZDebugDraw::PopulatePointsDrawList(bool dt, const float3& pos, f32 point_radius, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (dt)
                RZDebugDraw::Get().m_DrawListNDT.m_DebugPoints.emplace_back(pos, point_radius, colour);
            else
                RZDebugDraw::Get().m_DrawList.m_DebugPoints.emplace_back(pos, point_radius, colour);
        }

        void RZDebugDraw::PopulateThickLinesDrawList(bool DT, const float3& start, const float3& end, f32 line_width, const float4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (DT)
                RZDebugDraw::Get().m_DrawListNDT.m_DebugThickLines.emplace_back(start, end, colour);
            else
                RZDebugDraw::Get().m_DrawList.m_DebugThickLines.emplace_back(start, end, colour);
        }
    }    // namespace Gfx
}    // namespace Razix
