// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZDebugRenderer.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZFramebuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZRenderPass.h"
#include "Razix/Graphics/RHI/API/RZSwapchain.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Lighting/RZLight.h"

#include "Razix/Graphics/RHI/RHI.h"

namespace Razix {
    namespace Graphics {
        RZDebugRenderer* RZDebugRenderer::s_Instance = nullptr;

        static const u32 MaxPoints                  = 10000;
        static const u32 MaxPointVertices           = MaxPoints * 4;
        static const u32 MaxPointIndices            = MaxPoints * 6;
        static const u32 MAX_BATCH_DRAW_CALLS       = 100;
        static const u32 RENDERER_POINT_SIZE        = sizeof(Razix::Graphics::PointVertexData) * 4;
        static const u32 RENDERER_POINT_BUFFER_SIZE = RENDERER_POINT_SIZE * MaxPointVertices;

        static const uint32_t MaxLines                  = 10000;
        static const uint32_t MaxLineVertices           = MaxLines * 2;
        static const uint32_t MaxLineIndices            = MaxLines * 6;
        static const uint32_t MAX_LINE_BATCH_DRAW_CALLS = 100;
        static const uint32_t RENDERER_LINE_SIZE        = sizeof(Razix::Graphics::LineVertexData) * 4;
        static const uint32_t RENDERER_LINE_BUFFER_SIZE = RENDERER_LINE_SIZE * MaxLineVertices;

        Razix::Graphics::RZDebugRenderer* RZDebugRenderer::Get()
        {
            if (s_Instance == nullptr)
                s_Instance = new RZDebugRenderer;
            return s_Instance;
        }

        //---------------------------------------------------------------------------------------------------------------
        void RZDebugRenderer::Init()
        {
            for (sz i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
                m_MainCommandBuffers[i] = RZCommandBuffer::Create();
                m_MainCommandBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Debug Renderer Main Command Buffers"));
            }

            auto PointShader = Graphics::RZShaderLibrary::Get().getShader("DebugPoint.rzsf");

            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode               = Graphics::CullMode::NONE;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader                 = PointShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Graphics::RZTexture::Format::RGBA32F};
            pipelineInfo.depthFormat            = Graphics::RZTexture::Format::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            pipelineInfo.depthOp                = CompareOp::LessOrEqual;

            m_Pipeline = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Debug Renderer:: Points pipeline (DT)"));

            // Change the polygon mode for drawing lines
            auto LineShader          = Graphics::RZShaderLibrary::Get().getShader("DebugLine.rzsf");
            pipelineInfo.shader      = LineShader;
            pipelineInfo.cullMode    = CullMode::NONE;
            pipelineInfo.polygonMode = PolygonMode::FILL;
            pipelineInfo.drawType    = DrawType::LINES;
            m_LinePipeline           = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Debug Renderer:: Lines pipeline (DT)"));

            // Create the VBOs and IBOs
            // Points - Create a large enough to hold a large amount of points
            m_PointVBO = RZVertexBuffer::Create(RENDERER_POINT_BUFFER_SIZE, nullptr, Graphics::BufferUsage::DYNAMIC RZ_DEBUG_NAME_TAG_STR_E_ARG("Debug Points VBO"));
            m_LineVBO  = RZVertexBuffer::Create(RENDERER_LINE_BUFFER_SIZE, nullptr, Graphics::BufferUsage::DYNAMIC RZ_DEBUG_NAME_TAG_STR_E_ARG("Debug Lines VBO"));

            u32* indices = new u32[MaxPointIndices];
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

            m_PointIBO = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Debug Point IBO") indices, MaxPointIndices);

            delete[] indices;

            u32* line_indices = new u32[MaxLineIndices];

            for (uint32_t i = 0; i < MaxLineIndices; i++) {
                line_indices[i] = i;
            }

            m_LineIBO = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Debug Lines IBO") line_indices, MaxLineIndices);

            delete[] line_indices;
        }

        void RZDebugRenderer::Begin(RZScene* scene)
        {
            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            auto& sceneCamera = scene->getSceneCamera();

            auto cmdBuf = m_MainCommandBuffers[Graphics::RHI::GetSwapchain()->getCurrentImageIndex()];

            // Begin recording the command buffers
            Graphics::RHI::Begin(cmdBuf);

            RAZIX_MARK_BEGIN("Debug Renderer Pass", glm::vec4(0.0f, 0.85f, 0.0f, 1.0f));

            // Update the viewport
            cmdBuf->UpdateViewport(m_ScreenBufferWidth, m_ScreenBufferHeight);

            // POINTS
            {
                // Prepare the points VBO and IBO and update them
                m_PointVBO->Bind(cmdBuf);

                // Map the VBO
                m_PointVBO->Map(RENDERER_POINT_SIZE * static_cast<u32>(m_DrawList.m_DebugPoints.size()));
                PointVertexData* pointsVertexData = (PointVertexData*) m_PointVBO->GetMappedBuffer();

                for (auto& point: m_DrawList.m_DebugPoints) {
                    glm::vec3 right = point.size * sceneCamera.getRight();
                    glm::vec3 up    = point.size * sceneCamera.getUp();

                    pointsVertexData->vertex = glm::vec4(point.p1 - right - up, 1.0f);    // + glm::vec3(-point.size, -point.size, 0.0f));
                    pointsVertexData->colour = point.col;
                    pointsVertexData->size   = {point.size, 0.0f};
                    pointsVertexData->uv     = {-1.0f, -1.0f};
                    pointsVertexData++;

                    pointsVertexData->vertex = glm::vec4(point.p1 + right - up, 1.0f);    //(point.p1 + glm::vec3(point.size, -point.size, 0.0f));
                    pointsVertexData->colour = point.col;
                    pointsVertexData->size   = {point.size, 0.0f};
                    pointsVertexData->uv     = {1.0f, -1.0f};
                    pointsVertexData++;

                    pointsVertexData->vertex = glm::vec4(point.p1 + right + up, 1.0f);    //(point.p1 + glm::vec3(point.size, point.size, 0.0f));
                    pointsVertexData->colour = point.col;
                    pointsVertexData->size   = {point.size, 0.0f};
                    pointsVertexData->uv     = {1.0f, 1.0f};
                    pointsVertexData++;

                    pointsVertexData->vertex = glm::vec4(point.p1 - right + up, 1.0f);    // (point.p1 + glm::vec3(-point.size, point.size, 0.0f));
                    pointsVertexData->colour = point.col;
                    pointsVertexData->size   = {point.size, 0.0f};
                    pointsVertexData->uv     = {-1.0f, 1.0f};
                    pointsVertexData++;

                    m_PointIndexCount += 6;
                }

                m_PointVBO->UnMap();
                m_PointIBO->setCount(m_PointIndexCount);
            }

            // LINES
            {
                m_LineVBO->Bind(cmdBuf);

                m_LineVBO->Map(RENDERER_LINE_SIZE * static_cast<u32>(m_DrawList.m_DebugLines.size()));
                LineVertexData* lineVtxData = (LineVertexData*) m_LineVBO->GetMappedBuffer();

                for (auto& line: m_DrawList.m_DebugLines) {
                    lineVtxData->vertex = glm::vec4(line.p1, 1.0f);
                    lineVtxData->colour = line.col;
                    lineVtxData++;

                    lineVtxData->vertex = glm::vec4(line.p2, 1.0f);
                    lineVtxData->colour = line.col;
                    lineVtxData++;

                    m_LineIndexCount += 2;
                }

                m_LineVBO->UnMap();
                m_LineIBO->setCount(m_LineIndexCount);
            }
        }

        void RZDebugRenderer::Draw(RZCommandBuffer* cmdBuffer)
        {
            // Points
            {
                m_Pipeline->Bind(cmdBuffer);

                Graphics::RHI::BindDescriptorSets(m_Pipeline, cmdBuffer, &m_FrameDataSet, 1);

                m_PointVBO->Bind(cmdBuffer);
                m_PointIBO->Bind(cmdBuffer);

                RHI::DrawIndexed(cmdBuffer, m_PointIndexCount);
            }

            // Lines
            {
                m_LinePipeline->Bind(cmdBuffer);

                Graphics::RHI::BindDescriptorSets(m_LinePipeline, cmdBuffer, &m_FrameDataSet, 1);

                m_LineVBO->Bind(cmdBuffer);
                m_LineIBO->Bind(cmdBuffer);

                RHI::DrawIndexed(cmdBuffer, m_LineIndexCount);
            }
        }

        void RZDebugRenderer::End()
        {
            m_DrawList.m_DebugPoints.clear();
            m_DrawListNDT.m_DebugPoints.clear();
            m_PointIndexCount = 0;

            m_DrawList.m_DebugLines.clear();
            m_DrawListNDT.m_DebugLines.clear();
            m_LineIndexCount = 0;

            RAZIX_MARK_END();
        }

        void RZDebugRenderer::Resize(u32 width, u32 height)
        {
        }

        void RZDebugRenderer::Destroy()
        {
            if (m_FrameDataSet)
                m_FrameDataSet->Destroy();
            m_Pipeline->Destroy();
            m_LinePipeline->Destroy();
            m_PointIBO->Destroy();
            m_PointVBO->Destroy();
            m_LineIBO->Destroy();
            m_LineVBO->Destroy();
        }

        void RZDebugRenderer::SetFrameDataHeap(RZDescriptorSet* frameDataSet)
        {
            m_FrameDataSet = frameDataSet;
        }

        //---------------------------------------------------------------------------------------------------------------
        //Draw Point (circle)
        void RZDebugRenderer::GenDrawPoint(bool dt, const glm::vec3& pos, f32 point_radius, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (dt)
                s_Instance->m_DrawListNDT.m_DebugPoints.emplace_back(pos, point_radius, colour);
            else
                s_Instance->m_DrawList.m_DebugPoints.emplace_back(pos, point_radius, colour);
        }
        //---------------------------------------------------------------------------------------------------------------
        void RZDebugRenderer::DrawPoint(const glm::vec3& pos, f32 point_radius, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawPoint(false, pos, point_radius, glm::vec4(colour, 1.0f));
        }
        void RZDebugRenderer::DrawPoint(const glm::vec3& pos, f32 point_radius, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawPoint(false, pos, point_radius, colour);
        }
        void RZDebugRenderer::DrawPointDT(const glm::vec3& pos, f32 point_radius, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawPoint(true, pos, point_radius, glm::vec4(colour, 1.0f));
        }
        void RZDebugRenderer::DrawPointDT(const glm::vec3& pos, f32 point_radius, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawPoint(true, pos, point_radius, colour);
        }

        //---------------------------------------------------------------------------------------------------------------

        //Draw Line with a given thickness
        void RZDebugRenderer::GenDrawThickLine(bool DT, const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (DT)
                s_Instance->m_DrawListNDT.m_DebugThickLines.emplace_back(start, end, colour);
            else
                s_Instance->m_DrawList.m_DebugThickLines.emplace_back(start, end, colour);
        }
        void RZDebugRenderer::DrawThickLine(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawThickLine(false, start, end, line_width, glm::vec4(colour, 1.0f));
        }
        void RZDebugRenderer::DrawThickLine(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawThickLine(false, start, end, line_width, colour);
        }
        void RZDebugRenderer::DrawThickLineDT(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawThickLine(true, start, end, line_width, glm::vec4(colour, 1.0f));
        }
        void RZDebugRenderer::DrawThickLineDT(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawThickLine(true, start, end, line_width, colour);
        }

        //Draw line with thickness of 1 screen pixel regardless of distance from camera
        void RZDebugRenderer::GenDrawLine(bool dt, const glm::vec3& start, const glm::vec3& end, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (dt)
                s_Instance->m_DrawListNDT.m_DebugLines.emplace_back(start, end, colour);
            else
                s_Instance->m_DrawList.m_DebugLines.emplace_back(start, end, colour);
        }
        void RZDebugRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawLine(false, start, end, glm::vec4(colour, 1.0f));
        }
        void RZDebugRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawLine(false, start, end, colour);
        }
        void RZDebugRenderer::DrawLineDT(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawLine(true, start, end, glm::vec4(colour, 1.0f));
        }
        void RZDebugRenderer::DrawLineDT(const glm::vec3& start, const glm::vec3& end, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawLine(true, start, end, colour);
        }

        void RZDebugRenderer::DrawGrid(u32 dimension, const glm::vec4& colour)
        {
            float pos = float(dimension) / 2.0f;
            for (float x = -(float) dimension / 2.0f; x <= (float) dimension / 2.0f; x++) {
                DrawLine(glm::vec3(x, 0, -pos), glm::vec3(x, 0, pos), colour);
                DrawLine(glm::vec3(-pos, 0, x), glm::vec3(pos, 0, x), colour);
            }
        }

#if 0
        //Draw Matrix (x,y,z axis at pos)
        void RZDebugRenderer::DrawMatrix(const Maths::Matrix4& mtx)
        {
            //Maths::Vector3 position = mtx.Translation();
            //GenDrawHairLine(false, position, position + Maths::Vector3(mtx[0], mtx[1], mtx[2]), Maths::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
            //GenDrawHairLine(false, position, position + Maths::Vector3(mtx[4], mtx[5], mtx[6]), Maths::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
            //GenDrawHairLine(false, position, position + Maths::Vector3(mtx[8], mtx[9], mtx[10]), Maths::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        }
        void RZDebugRenderer::DrawMatrix(const Maths::Matrix3& mtx, const Maths::Vector3& position)
        {
            GenDrawHairLine(false, position, position + mtx.Column(0), Maths::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
            GenDrawHairLine(false, position, position + mtx.Column(1), Maths::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
            GenDrawHairLine(false, position, position + mtx.Column(2), Maths::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        }
        void RZDebugRenderer::DrawMatrixDT(const Maths::Matrix4& mtx)
        {
            //Maths::Vector3 position = mtx.Translation();
            //GenDrawHairLine(true, position, position + Maths::Vector3(mtx[0], mtx[1], mtx[2]), Maths::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
            //GenDrawHairLine(true, position, position + Maths::Vector3(mtx[4], mtx[5], mtx[6]), Maths::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
            //GenDrawHairLine(true, position, position + Maths::Vector3(mtx[8], mtx[9], mtx[10]), Maths::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        }
        void RZDebugRenderer::DrawMatrixDT(const Maths::Matrix3& mtx, const Maths::Vector3& position)
        {
            GenDrawHairLine(true, position, position + mtx.Column(0), Maths::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
            GenDrawHairLine(true, position, position + mtx.Column(1), Maths::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
            GenDrawHairLine(true, position, position + mtx.Column(2), Maths::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        }

        //Draw Triangle
        void RZDebugRenderer::GenDrawTriangle(bool DT, const Maths::Vector3& v0, const Maths::Vector3& v1, const Maths::Vector3& v2, const Maths::Vector4& colour)
        {
            if (s_Instance && s_Instance->m_Renderer2D)
                s_Instance->m_Renderer2D->SubmitTriangle(v0, v1, v2, colour);
        }

        void RZDebugRenderer::DrawTriangle(const Maths::Vector3& v0, const Maths::Vector3& v1, const Maths::Vector3& v2, const Maths::Vector4& colour)
        {
            GenDrawTriangle(false, v0, v1, v2, colour);
        }

        void RZDebugRenderer::DrawTriangleDT(const Maths::Vector3& v0, const Maths::Vector3& v1, const Maths::Vector3& v2, const Maths::Vector4& colour)
        {
            GenDrawTriangle(true, v0, v1, v2, colour);
        }

        //Draw Polygon (Renders as a triangle fan, so verts must be arranged in order)
        void RZDebugRenderer::DrawPolygon(int n_verts, const Maths::Vector3* verts, const Maths::Vector4& colour)
        {
            for (int i = 2; i < n_verts; ++i) {
                GenDrawTriangle(false, verts[0], verts[i - 1], verts[i], colour);
            }
        }

        void RZDebugRenderer::DrawPolygonDT(int n_verts, const Maths::Vector3* verts, const Maths::Vector4& colour)
        {
            for (int i = 2; i < n_verts; ++i) {
                GenDrawTriangle(true, verts[0], verts[i - 1], verts[i], colour);
            }
        }

        void RZDebugRenderer::DebugDraw(const Maths::BoundingBox& box, const Maths::Vector4& edgeColour, bool cornersOnly, f32 width)
        {
            Maths::Vector3 uuu = box.max_;
            Maths::Vector3 lll = box.min_;

            Maths::Vector3 ull(uuu.x, lll.y, lll.z);
            Maths::Vector3 uul(uuu.x, uuu.y, lll.z);
            Maths::Vector3 ulu(uuu.x, lll.y, uuu.z);

            Maths::Vector3 luu(lll.x, uuu.y, uuu.z);
            Maths::Vector3 llu(lll.x, lll.y, uuu.z);
            Maths::Vector3 lul(lll.x, uuu.y, lll.z);

            // Draw edges
            if (!cornersOnly) {
                DrawThickLineDT(luu, uuu, width, edgeColour);
                DrawThickLineDT(lul, uul, width, edgeColour);
                DrawThickLineDT(llu, ulu, width, edgeColour);
                DrawThickLineDT(lll, ull, width, edgeColour);

                DrawThickLineDT(lul, lll, width, edgeColour);
                DrawThickLineDT(uul, ull, width, edgeColour);
                DrawThickLineDT(luu, llu, width, edgeColour);
                DrawThickLineDT(uuu, ulu, width, edgeColour);

                DrawThickLineDT(lll, llu, width, edgeColour);
                DrawThickLineDT(ull, ulu, width, edgeColour);
                DrawThickLineDT(lul, luu, width, edgeColour);
                DrawThickLineDT(uul, uuu, width, edgeColour);
            } else {
                DrawThickLineDT(luu, luu + (uuu - luu) * 0.25f, width, edgeColour);
                DrawThickLineDT(luu + (uuu - luu) * 0.75f, uuu, width, edgeColour);

                DrawThickLineDT(lul, lul + (uul - lul) * 0.25f, width, edgeColour);
                DrawThickLineDT(lul + (uul - lul) * 0.75f, uul, width, edgeColour);

                DrawThickLineDT(llu, llu + (ulu - llu) * 0.25f, width, edgeColour);
                DrawThickLineDT(llu + (ulu - llu) * 0.75f, ulu, width, edgeColour);

                DrawThickLineDT(lll, lll + (ull - lll) * 0.25f, width, edgeColour);
                DrawThickLineDT(lll + (ull - lll) * 0.75f, ull, width, edgeColour);

                DrawThickLineDT(lul, lul + (lll - lul) * 0.25f, width, edgeColour);
                DrawThickLineDT(lul + (lll - lul) * 0.75f, lll, width, edgeColour);

                DrawThickLineDT(uul, uul + (ull - uul) * 0.25f, width, edgeColour);
                DrawThickLineDT(uul + (ull - uul) * 0.75f, ull, width, edgeColour);

                DrawThickLineDT(luu, luu + (llu - luu) * 0.25f, width, edgeColour);
                DrawThickLineDT(luu + (llu - luu) * 0.75f, llu, width, edgeColour);

                DrawThickLineDT(uuu, uuu + (ulu - uuu) * 0.25f, width, edgeColour);
                DrawThickLineDT(uuu + (ulu - uuu) * 0.75f, ulu, width, edgeColour);

                DrawThickLineDT(lll, lll + (llu - lll) * 0.25f, width, edgeColour);
                DrawThickLineDT(lll + (llu - lll) * 0.75f, llu, width, edgeColour);

                DrawThickLineDT(ull, ull + (ulu - ull) * 0.25f, width, edgeColour);
                DrawThickLineDT(ull + (ulu - ull) * 0.75f, ulu, width, edgeColour);

                DrawThickLineDT(lul, lul + (luu - lul) * 0.25f, width, edgeColour);
                DrawThickLineDT(lul + (luu - lul) * 0.75f, luu, width, edgeColour);

                DrawThickLineDT(uul, uul + (uuu - uul) * 0.25f, width, edgeColour);
                DrawThickLineDT(uul + (uuu - uul) * 0.75f, uuu, width, edgeColour);
            }
        }

        void RZDebugRenderer::DebugDraw(const Maths::Sphere& sphere, const Maths::Vector4& colour)
        {
            RAZIX::RZDebugRenderer::DrawPointDT(sphere.center_, sphere.radius_, colour);
        }

        void RZDebugRenderer::DebugDraw(const Maths::Frustum& frustum, const Maths::Vector4& colour)
        {
            auto* vertices = frustum.vertices_;

            RZDebugRenderer::DrawHairLine(vertices[0], vertices[1], colour);
            RZDebugRenderer::DrawHairLine(vertices[1], vertices[2], colour);
            RZDebugRenderer::DrawHairLine(vertices[2], vertices[3], colour);
            RZDebugRenderer::DrawHairLine(vertices[3], vertices[0], colour);
            RZDebugRenderer::DrawHairLine(vertices[4], vertices[5], colour);
            RZDebugRenderer::DrawHairLine(vertices[5], vertices[6], colour);
            RZDebugRenderer::DrawHairLine(vertices[6], vertices[7], colour);
            RZDebugRenderer::DrawHairLine(vertices[7], vertices[4], colour);
            RZDebugRenderer::DrawHairLine(vertices[0], vertices[4], colour);
            RZDebugRenderer::DrawHairLine(vertices[1], vertices[5], colour);
            RZDebugRenderer::DrawHairLine(vertices[2], vertices[6], colour);
            RZDebugRenderer::DrawHairLine(vertices[3], vertices[7], colour);
        }

        void RZDebugRenderer::DebugDraw(Graphics::Light* light, const Maths::Quaternion& rotation, const Maths::Vector4& colour)
        {
            //Directional
            if (light->Type < 0.1f) {
                auto           flipRotation = rotation * Maths::Quaternion::EulerAnglesToQuaternion(180.0f, 0.0f, 0.0f);
                Maths::Vector3 offset(0.0f, 0.1f, 0.0f);
                DrawHairLine((light->Position).ToVector3() + offset, (light->Position + (light->getDirection()) * 2.0f).ToVector3() + offset, colour);
                DrawHairLine((light->Position).ToVector3() - offset, (light->Position + (light->getDirection()) * 2.0f).ToVector3() - offset, colour);

                DrawHairLine((light->Position).ToVector3(), (light->Position + (light->getDirection()) * 2.0f).ToVector3(), colour);
                DebugDrawCone(20, 4, 30.0f, 1.5f, (light->Position - (light->getDirection()) * 1.5f).ToVector3(), flipRotation, colour);
            }
            //Spot
            else if (light->Type < 1.1f) {
                DebugDrawCone(20, 4, light->Angle * Maths::M_RADTODEG, light->Intensity, light->Position.ToVector3(), rotation, colour);
            }
            //Point
            else {
                DebugDrawSphere(light->Radius / 2.0f, light->Position.ToVector3(), colour);
            }
        }

        void RZDebugRenderer::DebugDraw(SoundNode* sound, const Maths::Vector4& colour)
        {
            DrawPoint(sound->GetPosition(), sound->GetRadius(), colour);
        }
#endif
        void RZDebugRenderer::DrawLight(Graphics::RZLight* light, const glm::vec4& colour)
        {
            // Directional
            if (light->getType() == LightType::DIRECTIONAL) {
                glm::vec3 offset(0.0f, 0.1f, 0.0f);
                DrawLine(glm::vec3(light->getPosition()) + offset, glm::vec3(light->getPosition() + (light->getDirection()) * 2.0f) + offset, colour);
                DrawLine(glm::vec3(light->getPosition()) - offset, glm::vec3(light->getPosition() + (light->getDirection()) * 2.0f) - offset, colour);

                DrawLine(glm::vec3(light->getPosition()), glm::vec3(light->getPosition() + (light->getDirection()) * 2.0f), colour);
                //DrawCone(20, 4, 30.0f, 1.5f, (light->getPosition() - (light->getDirection()) * 1.5f), rotation, colour);
            }
            //// Spot
            //else if (light->Type < 1.1f) {
            //    DrawCone(20, 4, light->getAngle(), light->getIntensity(), light->getPosition(), rotation, colour);
            //}
            //// Point
            //else {
            //    DrawSphere(light->Radius * 0.5f, light->getPosition(), colour);
            //}
            else if (light->getType() == LightType::Point) {
                DrawSphere(light->getRadius() * 0.5f, light->getPosition(), colour);
            }
        }

        void RZDebugRenderer::DrawCircle(int numVerts, f32 radius, const glm::vec3& position, const glm::vec3& eulerRotation, const glm::vec4& colour)
        {
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
#if 1

        void RZDebugRenderer::DrawCylinder(const glm::vec3& position, const glm::vec3& eulerRotation, float height, float radius, const glm::vec4& colour)
        {
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

        void RZDebugRenderer::DrawCapsule(const glm::vec3& position, const glm::vec3& eulerRotation, float height, float radius, const glm::vec4& colour)
        {
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

    #if 1
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
    #endif
            }
        }

        void RZDebugRenderer::DrawArc(int numVerts, float radius, const glm::vec3& start, const glm::vec3& end, const glm::vec3& eulerRotation, const glm::vec4& colour)
        {
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

        void RZDebugRenderer::DrawSphere(f32 radius, const glm::vec3& position, const glm::vec4& colour)
        {
            f32 offset = 0.0f;
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

        void RZDebugRenderer::DrawCone(int numCircleVerts, int numLinesToCircle, f32 angle, f32 length, const glm::vec3& position, const glm::vec3& rotation, const glm::vec4& colour)
        {
            f32       radius       = tan(glm::radians(angle * 0.5f)) * length;
            glm::quat quatRotation = glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)));

            glm::vec3 forward     = -(quatRotation * glm::vec3(0.0f, 0.0f, -1.0f));
            glm::vec3 endPosition = position + forward * length;
            f32       offset      = 0.0f;
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
#endif
    }    // namespace Graphics
}    // namespace Razix