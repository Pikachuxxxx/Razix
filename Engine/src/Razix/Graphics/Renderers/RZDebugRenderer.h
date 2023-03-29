#pragma once

#include "Razix/Core/RZSmartPointers.h"

#include "Razix/Graphics/Renderers/IRZRenderer.h"

#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

namespace Razix {
    namespace Graphics {

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
            static void DrawPointNDT(const glm::vec3& pos, f32 point_radius, const glm::vec3& colour);
            static void DrawPointNDT(const glm::vec3& pos, f32 point_radius, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

#if 0
            //Draw Line with a given thickness
            static void DrawThickLine(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec3& colour);
            static void DrawThickLine(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            static void DrawThickLineNDT(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec3& colour);
            static void DrawThickLineNDT(const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

            //Draw line with thickness of 1 screen pixel regardless of distance from camera
            static void DrawHairLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour);
            static void DrawHairLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            static void DrawHairLineNDT(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour);
            static void DrawHairLineNDT(const glm::vec3& start, const glm::vec3& end, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

            //Draw Matrix (x,y,z axis at pos)
            static void DrawMatrix(const Maths::Matrix4& transform_mtx);
            static void DrawMatrix(const Maths::Matrix3& rotation_mtx, const glm::vec3& position);
            static void DrawMatrixNDT(const Maths::Matrix4& transform_mtx);
            static void DrawMatrixNDT(const Maths::Matrix3& rotation_mtx, const glm::vec3& position);

            //Draw Triangle
            static void DrawTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            static void DrawTriangleNDT(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

            //Draw Polygon (Renders as a triangle fan, so verts must be arranged in order)
            static void DrawPolygon(int n_verts, const glm::vec3* verts, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            static void DrawPolygonNDT(int n_verts, const glm::vec3* verts, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

            static void DebugDraw(const Maths::BoundingBox& box, const glm::vec4& edgeColour, bool cornersOnly = false, f32 width = 0.02f);
            static void DebugDraw(const Maths::Sphere& sphere, const glm::vec4& colour);
            static void DebugDraw(const Maths::Frustum& frustum, const glm::vec4& colour);
            static void DebugDraw(Graphics::Light* light, const glm::quat& rotation, const glm::vec4& colour);
            static void DebugDrawSphere(f32 radius, const glm::vec3& position, const glm::vec4& colour);
            static void DebugDrawCircle(int numVerts, f32 radius, const glm::vec3& position, const glm::quat& rotation, const glm::vec4& colour);
            static void DebugDrawCone(int numCircleVerts, int numLinesToCircle, f32 angle, f32 length, const glm::vec3& position, const glm::quat& rotation, const glm::vec4& colour);
#endif

        protected:
            //Actual functions managing data parsing to save code bloat - called by public functions
            static void GenDrawPoint(bool ndt, const glm::vec3& pos, f32 point_radius, const glm::vec4& colour);
            static void GenDrawThickLine(bool ndt, const glm::vec3& start, const glm::vec3& end, f32 line_width, const glm::vec4& colour);
            static void GenDrawHairLine(bool ndt, const glm::vec3& start, const glm::vec3& end, const glm::vec4& colour);
            static void GenDrawTriangle(bool ndt, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& colour);

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
            DebugDrawList m_DrawListFG;

            RZShader*        m_PointShader;
            RZDescriptorSet* m_FrameDataSet;

            // VBs and IBs
            RZIndexBuffer*  m_PointIBO;
            RZVertexBuffer* m_PointVBO;
            u32             m_PointIndexCount = 0;
        };
    }    // namespace Graphics
}    // namespace Razix
