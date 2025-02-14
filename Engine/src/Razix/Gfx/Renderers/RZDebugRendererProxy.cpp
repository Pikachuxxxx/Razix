// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZDebugRendererProxy.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZSwapchain.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"
#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

#include "Razix/Gfx/RZMesh.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Lighting/RZLight.h"

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {

        static const u32 MaxQuadVerts = 4;
        static const u32 MaxLineVerts = 2;

        static const u32 MaxPoints                     = 10000;
        static const u32 MaxPointVertices              = MaxPoints * MaxQuadVerts;    // 4 vertices per quad
        static const u32 MaxPointIndices               = MaxPoints * 6;               // 6 indices per quad
        static const u32 PointPositionDataSize         = sizeof(glm::vec4);
        static const u32 PointColorDataSize            = sizeof(glm::vec4);
        static const u32 PointPositionVertexBufferSize = PointPositionDataSize * MaxPointVertices;
        static const u32 PointColorVertexBufferSize    = PointColorDataSize * MaxPointVertices;

        static const uint32_t MaxLines                     = 10000;
        static const uint32_t MaxLineVertices              = MaxLines * MaxLineVerts;    // 2 vertices per line
        static const uint32_t MaxLineIndices               = MaxLines * 2;               // 6 indices per quad since we draw a line as a very thin quad
        static const u32      LinePositionDataSize         = sizeof(glm::vec4);
        static const u32      LineColorDataSize            = sizeof(glm::vec4);
        static const u32      LinePositionVertexBufferSize = LinePositionDataSize * MaxLineVertices;
        static const u32      LineColorVertexBufferSize    = LineColorDataSize * MaxLineVertices;

        //---------------------------------------------------------------------------------------------------------------
        void RZDebugRendererProxy::Init()
        {
            createPipelines();

            // Create the VBOs and IBOs
            // Points - Create a large enough to hold a large amount of points
            createPointBufferResources();
            createLineBufferResources();
        }

        void RZDebugRendererProxy::Begin(RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            auto& sceneCamera = scene->getSceneCamera();

            RAZIX_MARK_BEGIN("Debug Renderer Pass", glm::vec4(0.0f, 0.85f, 0.0f, 1.0f));

            auto cmdBuffer = RHI::GetCurrentCommandBuffer();

            // POINTS
            {
                // Prepare the points VBO and IBO and update them

                auto pointPositionVBResource = RZResourceManager::Get().getVertexBufferResource(m_PointPosition_VB);
                auto pointColorVBResource    = RZResourceManager::Get().getVertexBufferResource(m_PointColor_VB);

                // Map the VBO
                pointPositionVBResource->Map(PointPositionDataSize * static_cast<u32>(m_DrawList.m_DebugPoints.size()) * MaxQuadVerts);
                glm::vec3* pointsVertexData = (glm::vec3*) pointPositionVBResource->GetMappedBuffer();

                pointColorVBResource->Map(PointColorDataSize * static_cast<u32>(m_DrawList.m_DebugPoints.size()) * MaxQuadVerts);
                glm::vec3* pointsColorData = (glm::vec3*) pointColorVBResource->GetMappedBuffer();

                u32 pointIdx = 0;
                for (auto& point: m_DrawList.m_DebugPoints) {
                    glm::vec3 right = point.size * sceneCamera.getRight();
                    glm::vec3 up    = point.size * sceneCamera.getUp();

                    // Define the four corners of the quad
                    glm::vec4 quadPositions[4] = {
                        glm::vec4(point.p1 - right - up, 1.0f),    // Bottom-left
                        glm::vec4(point.p1 + right - up, 1.0f),    // Bottom-right
                        glm::vec4(point.p1 + right + up, 1.0f),    // Top-right
                        glm::vec4(point.p1 - right + up, 1.0f)     // Top-left
                    };

                    glm::vec2 quadUVs[4] = {
                        {-1.0f, -1.0f},
                        {1.0f, -1.0f},
                        {1.0f, 1.0f},
                        {-1.0f, 1.0f}};

                    for (int i = 0; i < 4; i++) {
                        pointsVertexData[pointIdx] = quadPositions[i];
                        pointsColorData[pointIdx]  = point.col;
                        pointIdx++;
                    }

                    m_PointIndexCount += 6;    // For quad rendering
                }

                pointPositionVBResource->UnMap();
                pointColorVBResource->UnMap();

                auto pointIBResource = RZResourceManager::Get().getIndexBufferResource(m_PointIB);
                pointIBResource->setCount(m_PointIndexCount);
            }

            // LINES
            {
                auto linePositionVBResource = RZResourceManager::Get().getVertexBufferResource(m_LinePosition_VB);
                auto lineColorVBResource    = RZResourceManager::Get().getVertexBufferResource(m_LineColor_VB);

                // Map the VBOs
                linePositionVBResource->Map(LinePositionDataSize * static_cast<u32>(m_DrawList.m_DebugLines.size()) * MaxLineVertices);
                glm::vec4* linePositionData = static_cast<glm::vec4*>(linePositionVBResource->GetMappedBuffer());

                lineColorVBResource->Map(LineColorDataSize * static_cast<u32>(m_DrawList.m_DebugLines.size()) * MaxLineVertices);
                glm::vec4* lineColorData = static_cast<glm::vec4*>(lineColorVBResource->GetMappedBuffer());

                u32 lineIdx = 0;
                for (auto& line: m_DrawList.m_DebugLines) {
                    linePositionData[lineIdx] = glm::vec4(line.p1, 1.0f);
                    lineColorData[lineIdx]    = line.col;
                    lineIdx++;

                    linePositionData[lineIdx] = glm::vec4(line.p2, 1.0f);
                    lineColorData[lineIdx]    = line.col;
                    lineIdx++;

                    m_LineIndexCount += 2;
                }

                linePositionVBResource->UnMap();
                lineColorVBResource->UnMap();

                auto lineIBResource = RZResourceManager::Get().getIndexBufferResource(m_LineIB);
                lineIBResource->setCount(m_LineIndexCount);
            }
        }

        void RZDebugRendererProxy::Draw(RZDrawCommandBufferHandle cmdBuffer)
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

        void RZDebugRendererProxy::End()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_DrawList.m_DebugPoints.clear();
            m_DrawList.m_DebugLines.clear();
            m_DrawList.m_DebugThickLines.clear();
            m_PointIndexCount = 0;

            m_DrawListNDT.m_DebugPoints.clear();
            m_DrawListNDT.m_DebugLines.clear();
            m_DrawListNDT.m_DebugThickLines.clear();
            m_LineIndexCount = 0;

            RAZIX_MARK_END();
        }

        void RZDebugRendererProxy::Destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_LinePipeline);
            RZResourceManager::Get().destroyPipeline(m_PointPipeline);

            RZResourceManager::Get().destroyShader(m_PointShader);
            RZResourceManager::Get().destroyShader(m_LineShader);

            RZResourceManager::Get().destroyIndexBuffer(m_LineIB);
            RZResourceManager::Get().destroyVertexBuffer(m_LinePosition_VB);
            RZResourceManager::Get().destroyVertexBuffer(m_LineColor_VB);

            RZResourceManager::Get().destroyIndexBuffer(m_PointIB);
            RZResourceManager::Get().destroyVertexBuffer(m_PointPosition_VB);
            RZResourceManager::Get().destroyVertexBuffer(m_PointColor_VB);
        }

        //---------------------------------------------------------------------------------------------------------------

        void RZDebugRendererProxy::createPipelines()
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

        void RZDebugRendererProxy::createPointBufferResources()
        {
            RZBufferDesc pointVBDesc = {};
            pointVBDesc.data         = nullptr;
            pointVBDesc.usage        = BufferUsage::PersistentStream;

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

            RZBufferDesc pointIBDesc = {};
            pointIBDesc.name         = "IB_Points";
            pointIBDesc.usage        = BufferUsage::Static;
            pointIBDesc.count        = MaxPointIndices;
            pointIBDesc.data         = indices;
            m_PointIB                = RZResourceManager::Get().createIndexBuffer(pointIBDesc);

            Razix::Memory::RZFree(indices);
        }

        void RZDebugRendererProxy::createLineBufferResources()
        {
            RZBufferDesc lineVBDesc = {};
            lineVBDesc.usage        = BufferUsage::PersistentStream;
            lineVBDesc.data         = nullptr;

            lineVBDesc.name   = "VB_Lines::Positions";
            lineVBDesc.size   = LinePositionVertexBufferSize;
            m_LinePosition_VB = RZResourceManager::Get().createVertexBuffer(lineVBDesc);

            lineVBDesc.name = "VB_Lines::Colors";
            lineVBDesc.size = LinePositionVertexBufferSize;
            m_LineColor_VB  = RZResourceManager::Get().createVertexBuffer(lineVBDesc);

            //----------------------------------------

            u32* line_indices = (u32*) Razix::Memory::RZMalloc(MaxLineIndices * sizeof(u32));

            for (uint32_t i = 0; i < MaxLineIndices; i++) {
                line_indices[i] = i;
            }

            RZBufferDesc lineIBDesc = {};
            lineIBDesc.name         = "IB_Lines";
            lineIBDesc.usage        = BufferUsage::Static;
            lineIBDesc.count        = MaxLineIndices;
            lineIBDesc.data         = line_indices;
            m_LineIB                = RZResourceManager::Get().createIndexBuffer(lineIBDesc);

            Memory::RZFree(line_indices);
        }

        //---------------------------------------------------------------------------------------------------------------
        // Public API

        void RZDebugRendererProxy::DrawPoint(const glm::vec3& pos, f32 point_radius, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulatePointsDrawList(false, pos, point_radius, glm::vec4(colour, 1.0f));
        }
        void RZDebugRendererProxy::DrawPoint(const glm::vec3& pos, f32 point_radius, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulatePointsDrawList(false, pos, point_radius, colour);
        }
        void RZDebugRendererProxy::DrawPointDT(const glm::vec3& pos, f32 point_radius, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulatePointsDrawList(true, pos, point_radius, glm::vec4(colour, 1.0f));
        }
        void RZDebugRendererProxy::DrawPointDT(const glm::vec3& pos, f32 point_radius, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulatePointsDrawList(true, pos, point_radius, colour);
        }

        void RZDebugRendererProxy::DrawThickLine(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateThickLinesDrawList(false, start, end, line_width, glm::vec4(colour, 1.0f));
        }
        void RZDebugRendererProxy::DrawThickLine(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateThickLinesDrawList(false, start, end, line_width, colour);
        }
        void RZDebugRendererProxy::DrawThickLineDT(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateThickLinesDrawList(true, start, end, line_width, glm::vec4(colour, 1.0f));
        }
        void RZDebugRendererProxy::DrawThickLineDT(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateThickLinesDrawList(true, start, end, line_width, colour);
        }

        void RZDebugRendererProxy::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateLinesDrawList(false, start, end, glm::vec4(colour, 1.0f));
        }
        void RZDebugRendererProxy::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateLinesDrawList(false, start, end, colour);
        }
        void RZDebugRendererProxy::DrawLineDT(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateLinesDrawList(true, start, end, glm::vec4(colour, 1.0f));
        }
        void RZDebugRendererProxy::DrawLineDT(const glm::vec3& start, const glm::vec3& end, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            PopulateLinesDrawList(true, start, end, colour);
        }

        void RZDebugRendererProxy::DrawAABB(const Maths::AABB& box, const glm::vec4& edgeColour, bool cornersOnly /*= false*/, f32 width /*= 0.02f*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // 8 vertices
            glm::vec3 v1(box.min.x, box.min.y, box.min.z);
            glm::vec3 v2(box.min.x, box.min.y, box.max.z);
            glm::vec3 v3(box.min.x, box.max.y, box.min.z);
            glm::vec3 v4(box.min.x, box.max.y, box.max.z);
            glm::vec3 v5(box.max.x, box.min.y, box.min.z);
            glm::vec3 v6(box.max.x, box.min.y, box.max.z);
            glm::vec3 v7(box.max.x, box.max.y, box.min.z);
            glm::vec3 v8(box.max.x, box.max.y, box.max.z);

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

        void RZDebugRendererProxy::DrawGrid(u32 dimension, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            float pos = float(dimension) / 2.0f;
            for (float x = -(float) dimension / 2.0f; x <= (float) dimension / 2.0f; x++) {
                DrawLine(glm::vec3(x, 0, -pos), glm::vec3(x, 0, pos), colour);
                DrawLine(glm::vec3(-pos, 0, x), glm::vec3(pos, 0, x), colour);
            }
        }

        void RZDebugRendererProxy::DrawLight(Gfx::RZLight* light, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Directional
            if (light->getType() == LightType::DIRECTIONAL) {
                glm::vec3 offset(0.0f, 0.1f, 0.0f);
                auto      lightPos = glm::normalize(-light->getPosition());
                DrawLine(glm::vec3(light->getPosition()) + offset, glm::vec3(lightPos * 2.0f) + offset, colour);
                DrawLine(glm::vec3(light->getPosition()) - offset, glm::vec3(lightPos * 2.0f) - offset, colour);
                DrawLine(glm::vec3(light->getPosition()), glm::vec3(lightPos * 2.0f), colour);
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

        void RZDebugRendererProxy::DrawFrustum(const Maths::RZFrustum& frustum, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            const auto* vertices = frustum.getVertices();

            RZDebugRendererProxy::DrawLine(vertices[0], vertices[1], colour);
            RZDebugRendererProxy::DrawLine(vertices[1], vertices[2], colour);
            RZDebugRendererProxy::DrawLine(vertices[2], vertices[3], colour);
            RZDebugRendererProxy::DrawLine(vertices[3], vertices[0], colour);
            RZDebugRendererProxy::DrawLine(vertices[4], vertices[5], colour);
            RZDebugRendererProxy::DrawLine(vertices[5], vertices[6], colour);
            RZDebugRendererProxy::DrawLine(vertices[6], vertices[7], colour);
            RZDebugRendererProxy::DrawLine(vertices[7], vertices[4], colour);
            RZDebugRendererProxy::DrawLine(vertices[0], vertices[4], colour);
            RZDebugRendererProxy::DrawLine(vertices[1], vertices[5], colour);
            RZDebugRendererProxy::DrawLine(vertices[2], vertices[6], colour);
            RZDebugRendererProxy::DrawLine(vertices[3], vertices[7], colour);
        }

        void RZDebugRendererProxy::DrawFrustum(const glm::mat4& mat, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto frustum = Maths::RZFrustum();
            frustum.build(mat);

            const auto* vertices = frustum.getVertices();

            RZDebugRendererProxy::DrawLine(vertices[0], vertices[1], colour);
            RZDebugRendererProxy::DrawLine(vertices[1], vertices[2], colour);
            RZDebugRendererProxy::DrawLine(vertices[2], vertices[3], colour);
            RZDebugRendererProxy::DrawLine(vertices[3], vertices[0], colour);
            RZDebugRendererProxy::DrawLine(vertices[4], vertices[5], colour);
            RZDebugRendererProxy::DrawLine(vertices[5], vertices[6], colour);
            RZDebugRendererProxy::DrawLine(vertices[6], vertices[7], colour);
            RZDebugRendererProxy::DrawLine(vertices[7], vertices[4], colour);
            RZDebugRendererProxy::DrawLine(vertices[0], vertices[4], colour);
            RZDebugRendererProxy::DrawLine(vertices[1], vertices[5], colour);
            RZDebugRendererProxy::DrawLine(vertices[2], vertices[6], colour);
            RZDebugRendererProxy::DrawLine(vertices[3], vertices[7], colour);
        }

        void RZDebugRendererProxy::DrawCircle(int numVerts, f32 radius, const glm::vec3& position, const glm::vec3& eulerRotation, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            f32 sectorAngle = 360.0f / f32(numVerts);

            glm::quat rotation = glm::quat(glm::vec3(glm::radians(eulerRotation.x), glm::radians(eulerRotation.y), glm::radians(eulerRotation.z)));

            for (f32 angle = 0; angle <= 360.0f; angle += sectorAngle) {
                f32       cx      = cos(glm::radians(angle)) * radius;
                f32       cy      = sin(glm::radians(angle)) * radius;
                glm::vec3 current = glm::vec3(cx, cy, 0.0f);

                f32       nx   = cos(glm::radians(angle + sectorAngle)) * radius;
                f32       ny   = sin(glm::radians(angle + sectorAngle)) * radius;
                glm::vec3 next = glm::vec3(nx, ny, 0.0f);

                //DrawPoint(position + (current), 0.05, colour);
                DrawLine(position + (rotation * current), position + (rotation * next), colour);
            }
        }

        void RZDebugRendererProxy::DrawCylinder(const glm::vec3& position, const glm::vec3& eulerRotation, float height, float radius, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            glm::quat rotation = glm::quat(glm::vec3(glm::radians(eulerRotation.x), glm::radians(eulerRotation.y), glm::radians(eulerRotation.z)));

            glm::vec3 up = (rotation * glm::vec3(0.0f, 1.0f, 0.0f));

            glm::vec3 topSphereCentre    = position + up * (height * 0.5f);
            glm::vec3 bottomSphereCentre = position - up * (height * 0.5f);

            DrawCircle(20, radius, topSphereCentre, eulerRotation + glm::vec3(90.0f, 0.0f, 0.0f), colour);
            DrawCircle(20, radius, bottomSphereCentre, eulerRotation + glm::vec3(90.0f, 0.0f, 0.0f), colour);

            // Draw 10 arcs
            // Sides
            float step = 360.0f / float(20);
            for (int i = 0; i < 20; i++) {
                float z = cos(step * i) * radius;
                float x = sin(step * i) * radius;

                glm::vec3 offset = rotation * glm::vec4(x, 0.0f, z, 0.0f);
                DrawLine(bottomSphereCentre + offset, topSphereCentre + offset, colour);
            }
        }

        void RZDebugRendererProxy::DrawCapsule(const glm::vec3& position, const glm::vec3& eulerRotation, float height, float radius, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            glm::quat rotation = glm::quat(glm::vec3(glm::radians(eulerRotation.x), glm::radians(eulerRotation.y), glm::radians(eulerRotation.z)));

            glm::vec3 up = (rotation * glm::vec3(0.0f, 1.0f, 0.0f));

            glm::vec3 topSphereCentre    = position + up * (height * 0.5f);
            glm::vec3 bottomSphereCentre = position - up * (height * 0.5f);

            DrawCircle(20, radius, topSphereCentre, eulerRotation + glm::vec3(90.0f, 0.0f, 0.0f), colour);
            DrawCircle(20, radius, bottomSphereCentre, eulerRotation + glm::vec3(90.0f, 0.0f, 0.0f), colour);

            // Draw 10 arcs
            // Sides
            f32 sectorAngle = 360.0f / f32(10.0f);
            for (f32 angle = 0; angle <= 360.0f; angle += sectorAngle) {
                f32 x = cos(glm::radians(angle)) * radius;
                f32 z = sin(glm::radians(angle)) * radius;

                glm::vec3 offset = rotation * glm::vec4(x, 0.0f, z, 0.0f);
                DrawLine(bottomSphereCentre + offset, topSphereCentre + offset, colour);

                if (angle <= 180.0f) {
                    float z2 = cos(glm::radians(angle + 180.0f)) * radius;
                    float x2 = sin(glm::radians(angle + 180.0f)) * radius;

                    glm::vec3 offset2 = rotation * glm::vec4(x2, 0.0f, z2, 0.0f);
                    // Top HemiShpere
                    DrawArc(20, radius, topSphereCentre + offset, topSphereCentre + offset2, eulerRotation, colour);
                    DrawPoint(topSphereCentre + offset2, 0.1f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
                    // Bottom Hemisphere
                    //DebugDrawArc(20, radius, bottomSphereCentre + offset, bottomSphereCentre + offset, eulerRotation + glm::vec3(180.0f, 0.0f, 0.0f), colour);
                    DrawPoint(bottomSphereCentre + offset2, 0.1f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
                }
            }
        }

        void RZDebugRendererProxy::DrawArc(int numVerts, float radius, const glm::vec3& start, const glm::vec3& end, const glm::vec3& eulerRotation, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            f32 sectorAngle = 180.0f / f32(numVerts);

            glm::quat rotation = glm::quat(glm::vec3(glm::radians(eulerRotation.x), glm::radians(eulerRotation.y), glm::radians(eulerRotation.z)));

            glm::vec3 arcCentre = (start + end) * 0.5f;
            arcCentre           = arcCentre - (radius * 0.5f);
            for (f32 angle = 0; angle <= 180.0f; angle += sectorAngle) {
                f32       cx      = cos(glm::radians(angle)) * radius;
                f32       cy      = sin(glm::radians(angle)) * radius;
                glm::vec3 current = glm::vec3(cx, cy, 0.0f);

                f32       nx   = cos(glm::radians(angle + sectorAngle)) * radius;
                f32       ny   = sin(glm::radians(angle + sectorAngle)) * radius;
                glm::vec3 next = glm::vec3(nx, ny, 0.0f);

                //DrawPoint(position + (current), 0.05, colour);
                DrawLine(arcCentre + (rotation * current), arcCentre + (rotation * next), colour);
                DrawPoint(arcCentre + (rotation * current), 0.1f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            }
        }

        void RZDebugRendererProxy::DrawSphere(f32 radius, const glm::vec3& position, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            DrawCircle(50, radius, position, glm::vec3(90.0f, 0.0f, 0.0f), colour);
            DrawCircle(50, radius, position, glm::vec3(45.0f, 0.0f, 0.0f), colour);
            DrawCircle(50, radius, position, glm::vec3(-45.0f, 0.0f, 0.0f), colour);
            DrawCircle(50, radius, position, glm::vec3(0.0f, 90.0f, 0.0f), colour);
            DrawCircle(50, radius, position, glm::vec3(0.0f, 45.0f, 0.0f), colour);
            DrawCircle(50, radius, position, glm::vec3(00.0f, -45.0f, 0.0f), colour);
            DrawCircle(50, radius, position, glm::vec3(0.0f, 0.0f, 90.0f), colour);
            DrawCircle(50, radius, position, glm::vec3(0.0f, 0.0f, 45.0f), colour);
            DrawCircle(50, radius, position, glm::vec3(0.0f, 0.0f, -45.0f), colour);
        }

        void RZDebugRendererProxy::DrawCone(int numCircleVerts, int numLinesToCircle, f32 angle, f32 length, const glm::vec3& position, const glm::vec3& rotation, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            f32       radius       = tan(glm::radians(angle * 0.5f)) * length;
            glm::quat quatRotation = glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)));

            glm::vec3 forward     = -(quatRotation * glm::vec3(0.0f, 0.0f, -1.0f));
            glm::vec3 endPosition = position + forward * length;
            DrawCircle(numCircleVerts, radius, endPosition, rotation, colour);

            // FIXME: Use the draw circle logic and get the points on the circle and draw lines to it from the origin
            f32 sectorAngle = 360.0f / f32(numLinesToCircle);
            for (f32 sec_angle = 0; sec_angle <= 360.0f; sec_angle += sectorAngle) {
                f32 cx = cos(glm::radians(sec_angle)) * radius;
                f32 cy = sin(glm::radians(sec_angle)) * radius;

                glm::vec3 point    = glm::vec3(cx, cy, 0.0f);
                glm::vec3 endPoint = endPosition + point;
                DrawLine(position, endPoint, colour);
                //DrawPoint(endPoint,0.1f, colour);
            }
        }

        //---------------------------------------------------------------------------------------------------------------
        // Populate DrawList functions
        void RZDebugRendererProxy::PopulateLinesDrawList(bool dt, const glm::vec3& start, const glm::vec3& end, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (dt)
                RZDebugRendererProxy::Get().m_DrawListNDT.m_DebugLines.emplace_back(start, end, colour);
            else
                RZDebugRendererProxy::Get().m_DrawList.m_DebugLines.emplace_back(start, end, colour);
        }

        void RZDebugRendererProxy::PopulatePointsDrawList(bool dt, const glm::vec3& pos, f32 point_radius, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (dt)
                RZDebugRendererProxy::Get().m_DrawListNDT.m_DebugPoints.emplace_back(pos, point_radius, colour);
            else
                RZDebugRendererProxy::Get().m_DrawList.m_DebugPoints.emplace_back(pos, point_radius, colour);
        }

        void RZDebugRendererProxy::PopulateThickLinesDrawList(bool DT, const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (DT)
                RZDebugRendererProxy::Get().m_DrawListNDT.m_DebugThickLines.emplace_back(start, end, colour);
            else
                RZDebugRendererProxy::Get().m_DrawList.m_DebugThickLines.emplace_back(start, end, colour);
        }

    }    // namespace Gfx
}    // namespace Razix
