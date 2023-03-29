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

        static const u32 MaxLines        = 10000;
        static const u32 MaxLineVertices = MaxLines * 2;
        static const u32 MaxLineIndices  = MaxLines * 6;

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
                m_MainCommandBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Forward Renderer Main Command Buffers"));
            }

            m_PointShader = Graphics::RZShaderLibrary::Get().getShader("DebugPoint.rzsf");

            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode               = Graphics::CullMode::NONE;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader                 = m_PointShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Graphics::RZTexture::Format::RGBA32F};
            pipelineInfo.depthFormat            = Graphics::RZTexture::Format::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;

            m_Pipeline = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Debug Renderer Pipeline (Depth Test enabled)"));

            // Create the VBOs and IBOs
            // Points - Create a large enough to hold a large amount of points
            m_PointVBO = RZVertexBuffer::Create(RENDERER_POINT_BUFFER_SIZE, nullptr, Graphics::BufferUsage::DYNAMIC RZ_DEBUG_NAME_TAG_STR_E_ARG("Debug Points VBO"));

            u16* indices = new u16[MaxPointIndices];
            u16  offset  = 0;
            for (u16 i = 0; i < MaxPointIndices; i += 6) {
                indices[i]     = offset + 0;
                indices[i + 1] = offset + 1;
                indices[i + 2] = offset + 2;

                indices[i + 3] = offset + 2;
                indices[i + 4] = offset + 3;
                indices[i + 5] = offset + 0;

                offset += 4;
            }

            m_PointIBO = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Debug Point IBO") indices, MaxPointIndices);
        }

        void RZDebugRenderer::Begin(RZScene* scene)
        {
            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            auto& sceneCamera = scene->getSceneCamera();

            auto cmdBuf = m_MainCommandBuffers[Graphics::RHI::getSwapchain()->getCurrentImageIndex()];

            // Begin recording the command buffers
            Graphics::RHI::Begin(cmdBuf);

            RAZIX_MARK_BEGIN("Debug Renderer Pass", glm::vec4(0.0f, 0.85f, 0.0f, 1.0f));

            // Update the viewport
            cmdBuf->UpdateViewport(m_ScreenBufferWidth, m_ScreenBufferHeight);

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
        }

        void RZDebugRenderer::End()
        {
            m_DrawList.m_DebugPoints.clear();
            m_PointIndexCount = 0;
        }

        void RZDebugRenderer::Resize(u32 width, u32 height)
        {
        }

        void RZDebugRenderer::Destroy()
        {
        }
        //---------------------------------------------------------------------------------------------------------------
        //Draw Point (circle)
        void RZDebugRenderer::GenDrawPoint(bool ndt, const glm::vec3& pos, f32 point_radius, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (ndt)
                s_Instance->m_DrawListFG.m_DebugPoints.emplace_back(pos, point_radius, colour);
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
        void RZDebugRenderer::DrawPointNDT(const glm::vec3& pos, f32 point_radius, const glm::vec3& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawPoint(true, pos, point_radius, glm::vec4(colour, 1.0f));
        }
        void RZDebugRenderer::DrawPointNDT(const glm::vec3& pos, f32 point_radius, const glm::vec4& colour)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            GenDrawPoint(true, pos, point_radius, colour);
        }

        void RZDebugRenderer::SetFrameDataHeap(RZDescriptorSet* frameDataSet)
        {
            m_FrameDataSet = frameDataSet;
        }

        //---------------------------------------------------------------------------------------------------------------

#if 0
  //Draw Line with a given thickness
        void RZDebugRenderer::GenDrawThickLine(bool ndt, const Maths::Vector3& start, const Maths::Vector3& end, f32 line_width, const Maths::Vector4& colour)
        {
            if (s_Instance && s_Instance->m_LineRenderer)
                s_Instance->m_LineRenderer->Submit(start, end, colour);
        }
        void RZDebugRenderer::DrawThickLine(const Maths::Vector3& start, const Maths::Vector3& end, f32 line_width, const Maths::Vector3& colour)
        {
            GenDrawThickLine(false, start, end, line_width, Maths::Vector4(colour, 1.0f));
        }
        void RZDebugRenderer::DrawThickLine(const Maths::Vector3& start, const Maths::Vector3& end, f32 line_width, const Maths::Vector4& colour)
        {
            GenDrawThickLine(false, start, end, line_width, colour);
        }
        void RZDebugRenderer::DrawThickLineNDT(const Maths::Vector3& start, const Maths::Vector3& end, f32 line_width, const Maths::Vector3& colour)
        {
            GenDrawThickLine(true, start, end, line_width, Maths::Vector4(colour, 1.0f));
        }
        void RZDebugRenderer::DrawThickLineNDT(const Maths::Vector3& start, const Maths::Vector3& end, f32 line_width, const Maths::Vector4& colour)
        {
            GenDrawThickLine(true, start, end, line_width, colour);
        }

        //Draw line with thickness of 1 screen pixel regardless of distance from camera
        void RZDebugRenderer::GenDrawHairLine(bool ndt, const Maths::Vector3& start, const Maths::Vector3& end, const Maths::Vector4& colour)
        {
            if (s_Instance && s_Instance->m_LineRenderer)
                s_Instance->m_LineRenderer->Submit(start, end, colour);
        }
        void RZDebugRenderer::DrawHairLine(const Maths::Vector3& start, const Maths::Vector3& end, const Maths::Vector3& colour)
        {
            GenDrawHairLine(false, start, end, Maths::Vector4(colour, 1.0f));
        }
        void RZDebugRenderer::DrawHairLine(const Maths::Vector3& start, const Maths::Vector3& end, const Maths::Vector4& colour)
        {
            GenDrawHairLine(false, start, end, colour);
        }
        void RZDebugRenderer::DrawHairLineNDT(const Maths::Vector3& start, const Maths::Vector3& end, const Maths::Vector3& colour)
        {
            GenDrawHairLine(true, start, end, Maths::Vector4(colour, 1.0f));
        }
        void RZDebugRenderer::DrawHairLineNDT(const Maths::Vector3& start, const Maths::Vector3& end, const Maths::Vector4& colour)
        {
            GenDrawHairLine(true, start, end, colour);
        }

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
        void RZDebugRenderer::DrawMatrixNDT(const Maths::Matrix4& mtx)
        {
            //Maths::Vector3 position = mtx.Translation();
            //GenDrawHairLine(true, position, position + Maths::Vector3(mtx[0], mtx[1], mtx[2]), Maths::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
            //GenDrawHairLine(true, position, position + Maths::Vector3(mtx[4], mtx[5], mtx[6]), Maths::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
            //GenDrawHairLine(true, position, position + Maths::Vector3(mtx[8], mtx[9], mtx[10]), Maths::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        }
        void RZDebugRenderer::DrawMatrixNDT(const Maths::Matrix3& mtx, const Maths::Vector3& position)
        {
            GenDrawHairLine(true, position, position + mtx.Column(0), Maths::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
            GenDrawHairLine(true, position, position + mtx.Column(1), Maths::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
            GenDrawHairLine(true, position, position + mtx.Column(2), Maths::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        }

        //Draw Triangle
        void RZDebugRenderer::GenDrawTriangle(bool ndt, const Maths::Vector3& v0, const Maths::Vector3& v1, const Maths::Vector3& v2, const Maths::Vector4& colour)
        {
            if (s_Instance && s_Instance->m_Renderer2D)
                s_Instance->m_Renderer2D->SubmitTriangle(v0, v1, v2, colour);
        }

        void RZDebugRenderer::DrawTriangle(const Maths::Vector3& v0, const Maths::Vector3& v1, const Maths::Vector3& v2, const Maths::Vector4& colour)
        {
            GenDrawTriangle(false, v0, v1, v2, colour);
        }

        void RZDebugRenderer::DrawTriangleNDT(const Maths::Vector3& v0, const Maths::Vector3& v1, const Maths::Vector3& v2, const Maths::Vector4& colour)
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

        void RZDebugRenderer::DrawPolygonNDT(int n_verts, const Maths::Vector3* verts, const Maths::Vector4& colour)
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
                DrawThickLineNDT(luu, uuu, width, edgeColour);
                DrawThickLineNDT(lul, uul, width, edgeColour);
                DrawThickLineNDT(llu, ulu, width, edgeColour);
                DrawThickLineNDT(lll, ull, width, edgeColour);

                DrawThickLineNDT(lul, lll, width, edgeColour);
                DrawThickLineNDT(uul, ull, width, edgeColour);
                DrawThickLineNDT(luu, llu, width, edgeColour);
                DrawThickLineNDT(uuu, ulu, width, edgeColour);

                DrawThickLineNDT(lll, llu, width, edgeColour);
                DrawThickLineNDT(ull, ulu, width, edgeColour);
                DrawThickLineNDT(lul, luu, width, edgeColour);
                DrawThickLineNDT(uul, uuu, width, edgeColour);
            } else {
                DrawThickLineNDT(luu, luu + (uuu - luu) * 0.25f, width, edgeColour);
                DrawThickLineNDT(luu + (uuu - luu) * 0.75f, uuu, width, edgeColour);

                DrawThickLineNDT(lul, lul + (uul - lul) * 0.25f, width, edgeColour);
                DrawThickLineNDT(lul + (uul - lul) * 0.75f, uul, width, edgeColour);

                DrawThickLineNDT(llu, llu + (ulu - llu) * 0.25f, width, edgeColour);
                DrawThickLineNDT(llu + (ulu - llu) * 0.75f, ulu, width, edgeColour);

                DrawThickLineNDT(lll, lll + (ull - lll) * 0.25f, width, edgeColour);
                DrawThickLineNDT(lll + (ull - lll) * 0.75f, ull, width, edgeColour);

                DrawThickLineNDT(lul, lul + (lll - lul) * 0.25f, width, edgeColour);
                DrawThickLineNDT(lul + (lll - lul) * 0.75f, lll, width, edgeColour);

                DrawThickLineNDT(uul, uul + (ull - uul) * 0.25f, width, edgeColour);
                DrawThickLineNDT(uul + (ull - uul) * 0.75f, ull, width, edgeColour);

                DrawThickLineNDT(luu, luu + (llu - luu) * 0.25f, width, edgeColour);
                DrawThickLineNDT(luu + (llu - luu) * 0.75f, llu, width, edgeColour);

                DrawThickLineNDT(uuu, uuu + (ulu - uuu) * 0.25f, width, edgeColour);
                DrawThickLineNDT(uuu + (ulu - uuu) * 0.75f, ulu, width, edgeColour);

                DrawThickLineNDT(lll, lll + (llu - lll) * 0.25f, width, edgeColour);
                DrawThickLineNDT(lll + (llu - lll) * 0.75f, llu, width, edgeColour);

                DrawThickLineNDT(ull, ull + (ulu - ull) * 0.25f, width, edgeColour);
                DrawThickLineNDT(ull + (ulu - ull) * 0.75f, ulu, width, edgeColour);

                DrawThickLineNDT(lul, lul + (luu - lul) * 0.25f, width, edgeColour);
                DrawThickLineNDT(lul + (luu - lul) * 0.75f, luu, width, edgeColour);

                DrawThickLineNDT(uul, uul + (uuu - uul) * 0.25f, width, edgeColour);
                DrawThickLineNDT(uul + (uuu - uul) * 0.75f, uuu, width, edgeColour);
            }
        }

        void RZDebugRenderer::DebugDraw(const Maths::Sphere& sphere, const Maths::Vector4& colour)
        {
            RAZIX::RZDebugRenderer::DrawPointNDT(sphere.center_, sphere.radius_, colour);
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

        void RZDebugRenderer::Begin()
        {
            if (m_LineRenderer)
                m_LineRenderer->Begin();
            if (m_Renderer2D)
                m_Renderer2D->BeginSimple();
            if (m_PointRenderer)
                m_PointRenderer->Begin();
        }

        void RZDebugRenderer::ClearInternal()
        {
            if (m_Renderer2D)
                m_Renderer2D->Clear();
            if (m_PointRenderer)
                m_PointRenderer->Clear();
            if (m_LineRenderer)
                m_LineRenderer->Clear();
        }

        void RZDebugRenderer::BeginSceneInternal(Scene* scene, Camera* overrideCamera, Maths::Transform* overrideCameraTransform)
        {
            RAZIX_PROFILE_FUNCTION();
            if (m_Renderer2D) {
                m_Renderer2D->BeginScene(scene, overrideCamera, overrideCameraTransform);
            }

            if (m_PointRenderer)
                m_PointRenderer->BeginScene(scene, overrideCamera, overrideCameraTransform);
            if (m_LineRenderer)
                m_LineRenderer->BeginScene(scene, overrideCamera, overrideCameraTransform);
        }

        void RZDebugRenderer::RenderInternal()
        {
            RAZIX_PROFILE_FUNCTION();
            if (m_Renderer2D) {
                m_Renderer2D->Begin();
                m_Renderer2D->SetSystemUniforms(m_Renderer2D->GetShader().get());
                m_Renderer2D->SubmitTriangles();
                m_Renderer2D->Present();
                m_Renderer2D->End();
            }

            if (m_PointRenderer)
                m_PointRenderer->RenderInternal();
            if (m_LineRenderer)
                m_LineRenderer->RenderInternal();
        }

        void RZDebugRenderer::OnResizeInternal(u32 width, u32 height)
        {
            if (m_Renderer2D)
                m_Renderer2D->OnResize(width, height);
            if (m_LineRenderer)
                m_LineRenderer->OnResize(width, height);
            if (m_PointRenderer)
                m_PointRenderer->OnResize(width, height);
        }

        void RZDebugRenderer::DebugDraw(Graphics::Light* light, const Maths::Quaternion& rotation, const Maths::Vector4& colour)
        {
            //Directional
            if (light->Type < 0.1f) {
                auto           flipRotation = rotation * Maths::Quaternion::EulerAnglesToQuaternion(180.0f, 0.0f, 0.0f);
                Maths::Vector3 offset(0.0f, 0.1f, 0.0f);
                DrawHairLine((light->Position).ToVector3() + offset, (light->Position + (light->Direction) * 2.0f).ToVector3() + offset, colour);
                DrawHairLine((light->Position).ToVector3() - offset, (light->Position + (light->Direction) * 2.0f).ToVector3() - offset, colour);

                DrawHairLine((light->Position).ToVector3(), (light->Position + (light->Direction) * 2.0f).ToVector3(), colour);
                DebugDrawCone(20, 4, 30.0f, 1.5f, (light->Position - (light->Direction) * 1.5f).ToVector3(), flipRotation, colour);
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

        void RZDebugRenderer::DebugDrawCircle(int numVerts, f32 radius, const Maths::Vector3& position, const Maths::Quaternion& rotation, const Maths::Vector4& colour)
        {
            f32 step = 360.0f / f32(numVerts);

            for (int i = 0; i < numVerts; i++) {
                f32            cx      = Maths::Cos(step * i) * radius;
                f32            cy      = Maths::Sin(step * i) * radius;
                Maths::Vector3 current = Maths::Vector3(cx, cy);

                f32            nx   = Maths::Cos(step * (i + 1)) * radius;
                f32            ny   = Maths::Sin(step * (i + 1)) * radius;
                Maths::Vector3 next = Maths::Vector3(nx, ny);

                DrawHairLine(position + (rotation * current), position + (rotation * next), colour);
            }
        }
        void RZDebugRenderer::DebugDrawSphere(f32 radius, const Maths::Vector3& position, const Maths::Vector4& colour)
        {
            f32 offset = 0.0f;
            DebugDrawCircle(20, radius, position, Maths::Quaternion::EulerAnglesToQuaternion(0.0f, 0.0f, 0.0f), colour);
            DebugDrawCircle(20, radius, position, Maths::Quaternion::EulerAnglesToQuaternion(90.0f, 0.0f, 0.0f), colour);
            DebugDrawCircle(20, radius, position, Maths::Quaternion::EulerAnglesToQuaternion(0.0f, 90.0f, 90.0f), colour);
        }

        void RZDebugRenderer::DebugDrawCone(int numCircleVerts, int numLinesToCircle, f32 angle, f32 length, const Maths::Vector3& position, const Maths::Quaternion& rotation, const Maths::Vector4& colour)
        {
            f32            endAngle    = Maths::Tan(angle * 0.5f) * length;
            Maths::Vector3 forward     = -(rotation * Maths::Vector3::FORWARD);
            Maths::Vector3 endPosition = position + forward * length;
            f32            offset      = 0.0f;
            DebugDrawCircle(numCircleVerts, endAngle, endPosition, rotation, colour);

            for (int i = 0; i < numLinesToCircle; i++) {
                f32            a     = i * 90.0f;
                Maths::Vector3 point = rotation * Maths::Vector3(Maths::Cos(a), Maths::Sin(a)) * endAngle;
                DrawHairLine(position, position + point + forward * length, colour);
            }
        }
#endif
    }    // namespace Graphics
}    // namespace Razix