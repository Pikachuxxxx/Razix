#pragma once

#include "Razix/Core/RZSmartPointers.h"

#include "Razix/Graphics/Renderers/IRZRenderer.h"

#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Maths/AABB.h"
#include "Razix/Maths/Frustum.h"

namespace Razix {
    namespace Graphics {

        class RZLight;
        class RZDescriptorSet;

        struct Line
        {
            glm::vec3 p1;
            glm::vec3 p2;
            glm::vec4 col;

            Line(const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec4& colour)
            {
                p1  = pos1;
                p2  = pos2;
                col = colour;
            }
        };

        struct Point
        {
            glm::vec3 p1;
            glm::vec4 col;
            float     size;

            Point(const glm::vec3& pos1, float s, const glm::vec4& colour)
            {
                p1   = pos1;
                size = s;
                col  = colour;
            }
        };

        struct Triangle
        {
            glm::vec3 p1;
            glm::vec3 p2;
            glm::vec3 p3;
            glm::vec4 col;

            Triangle(const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3, const glm::vec4& colour)
            {
                p1  = pos1;
                p2  = pos2;
                p3  = pos3;
                col = colour;
            }
        };

        struct LineVertexData
        {
            glm::vec4 vertex;
            glm::vec4 colour;

            bool operator==(const LineVertexData& other) const
            {
                return vertex == other.vertex && colour == other.colour;
            }
        };

        struct PointVertexData
        {
            glm::vec4 vertex;
            glm::vec4 colour;
            glm::vec2 uv;
            glm::vec2 size;

            bool operator==(const PointVertexData& other) const
            {
                return vertex == other.vertex && colour == other.colour && size == other.size && uv == other.uv;
            }
        };

        /**
         * Draws debug geometry in the scene to help with visualization and debugging
         * 
         * Note: Uses Batched rendering to draw points/lines in a single draw call
         */
        class RAZIX_API RZDebugRenderer : public IRZRenderer
        {
        public:
            RZDebugRenderer()  = default;
            ~RZDebugRenderer() = default;

            static RZDebugRenderer* Get();

            //-------------------------------------------------------------
            // IRZRenderer

            void Init() override;

            void Begin(RZScene* scene) override;

            void Draw(RZCommandBuffer* cmdBuffer) override;

            void End() override;

            void Resize(u32 width, u32 height) override;

            void Destroy() override;

            void SetFrameDataHeap(RZDescriptorSet* frameDataSet) override;

            //-------------------------------------------------------------

            //Note: Functions appended with 'FG' (no depth testing) will always be rendered in the foreground. This can be useful for debugging things inside objects.

            //Draw Point (very small circle)
            static void DrawPoint(const glm::vec3& pos, f32 point_radius, const glm::vec3& colour);
            static void DrawPoint(const glm::vec3& pos, f32 point_radius, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            static void DrawPointDT(const glm::vec3& pos, f32 point_radius, const glm::vec3& colour);
            static void DrawPointDT(const glm::vec3& pos, f32 point_radius, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

            //Draw Line with a given thickness
            static void DrawThickLine(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec3& colour);
            static void DrawThickLine(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            static void DrawThickLineDT(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec3& colour);
            static void DrawThickLineDT(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

            //Draw line with thickness of 1 screen pixel regardless of distance from camera
            static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour);
            static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            static void DrawLineDT(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour);
            static void DrawLineDT(const glm::vec3& start, const glm::vec3& end, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

#if 0
            //Draw Matrix (x,y,z axis at pos)
            static void DrawMatrix(const glm::mat4& transform_mtx);
            static void DrawMatrix(const glm::mat3& rotation_mtx, const glm::vec3& position);
            static void DrawMatrixNDT(const glm::mat4& transform_mtx);
            static void DrawMatrixNDT(const glm::mat3& rotation_mtx, const glm::vec3& position);
#endif

            static void DrawAABB(const Maths::AABB& box, const glm::vec4& edgeColour, bool cornersOnly = false, f32 width = 0.02f);
            static void DrawGrid(u32 dimension, const glm::vec4& colour);

            static void DrawLight(Graphics::RZLight* light, const glm::vec4& colour);
            static void DrawFrustum(const Maths::Frustum& frustum, const glm::vec4& colour);
            static void DrawCylinder(const glm::vec3& position, const glm::vec3& eulerRotation, float height, float radius, const glm::vec4& colour);
            static void DrawCapsule(const glm::vec3& position, const glm::vec3& eulerRotation, float height, float radius, const glm::vec4& colour);

            static void DrawArc(int numVerts, float radius, const glm::vec3& start, const glm::vec3& end, const glm::vec3& eulerRotation, const glm::vec4& colour);

            static void DrawSphere(f32 radius, const glm::vec3& position, const glm::vec4& colour);
            static void DrawCircle(int numVerts, f32 radius, const glm::vec3& position, const glm::vec3& eulerRotation, const glm::vec4& colour);
            static void DrawCone(int numCircleVerts, int numLinesToCircle, f32 angle, f32 length, const glm::vec3& position, const glm::vec3& rotation, const glm::vec4& colour);

        protected:
            //Actual functions managing data parsing to save code bloat - called by public functions
            static void GenDrawPoint(bool dt, const glm::vec3& pos, f32 point_radius, const glm::vec4& colour);
            static void GenDrawThickLine(bool dt, const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec4& colour);
            static void GenDrawLine(bool dt, const glm::vec3& start, const glm::vec3& end, const glm::vec4& colour);
            static void GenDrawTriangle(bool dt, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& colour);

        private:
            static RZDebugRenderer* s_Instance;

            struct DebugDrawList
            {
                std::vector<Triangle> m_DebugTriangles;
                std::vector<Line>     m_DebugLines;
                std::vector<Point>    m_DebugPoints;
                std::vector<Line>     m_DebugThickLines;
            };

            DebugDrawList m_DrawList;
            DebugDrawList m_DrawListNDT;

            RZDescriptorSet* m_FrameDataSet = nullptr;
            RZPipeline*      m_LinePipeline = nullptr;

            // VBs and IBs
            RZIndexBuffer*  m_PointIBO        = nullptr;
            RZVertexBuffer* m_PointVBO        = nullptr;
            u32             m_PointIndexCount = 0;
            RZIndexBuffer*  m_LineIBO         = nullptr;
            RZVertexBuffer* m_LineVBO         = nullptr;
            u32             m_LineIndexCount  = 0;
        };
    }    // namespace Graphics
}    // namespace Razix
