#pragma once

// Modified from Source: https://github.com/jmorton06/Lumos/blob/main/Lumos/Source/Lumos/Graphics/Renderers/DebugRenderer.cpp // MIT License

#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/Containers/smart_pointers.h"
#include "Razix/Core/Utils/TRZSingleton.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Math/AABB.h"
#include "Razix/Math/Frustum.h"

namespace Razix {
    namespace Gfx {
        
        // Forward Declarations
        class RZLight;

        /**
         * Draws debug geometry in the scene to help with visualization and debugging
         * 
         * Note: Uses Batched rendering to draw points/lines in a single draw call
         */
        class RAZIX_API RZDebugDraw 
        {
        public:
            //-------------------------------------------------------------
            static void Init();
            static void Destroy();
            static void BeginDraw();
            static void IssueDrawCommands(rz_gfx_cmdbuf_handle cmdBuffer);
            static void EndDraw();

            //-------------------------------------------------------------
            // Debug Draw Public API

            //Draw Point (very small circle)
            static void DrawPoint(const float3& pos, f32 point_radius, const float3& colour);
            static void DrawPoint(const float3& pos, f32 point_radius, const float4& colour = float4(1.0f, 1.0f, 1.0f, 1.0f));
            static void DrawPointDT(const float3& pos, f32 point_radius, const float3& colour);
            static void DrawPointDT(const float3& pos, f32 point_radius, const float4& colour = float4(1.0f, 1.0f, 1.0f, 1.0f));

            //Draw Line with a given thickness
            static void DrawThickLine(const float3& start, const float3& end, f32 line_width, const float3& colour);
            static void DrawThickLine(const float3& start, const float3& end, f32 line_width, const float4& colour = float4(1.0f, 1.0f, 1.0f, 1.0f));
            static void DrawThickLineDT(const float3& start, const float3& end, f32 line_width, const float3& colour);
            static void DrawThickLineDT(const float3& start, const float3& end, f32 line_width, const float4& colour = float4(1.0f, 1.0f, 1.0f, 1.0f));

            //Draw line with thickness of 1 screen pixel regardless of distance from camera
            static void DrawLine(const float3& start, const float3& end, const float3& colour);
            static void DrawLine(const float3& start, const float3& end, const float4& colour = float4(1.0f, 1.0f, 1.0f, 1.0f));
            static void DrawLineDT(const float3& start, const float3& end, const float3& colour);
            static void DrawLineDT(const float3& start, const float3& end, const float4& colour = float4(1.0f, 1.0f, 1.0f, 1.0f));

            static void DrawAABB(const Maths::AABB& box, const float4& edgeColour, bool cornersOnly = false, f32 width = 0.02f);
            static void DrawGrid(u32 dimension, const float4& colour);

            static void DrawLight(Gfx::RZLight* light, const float4& colour);
            static void DrawFrustum(const Maths::RZFrustum& frustum, const float4& colour);
            static void DrawFrustum(const float4x4& mat, const float4& colour);
            static void DrawCylinder(const float3& position, const float3& eulerRotation, float height, float radius, const float4& colour);
            static void DrawCapsule(const float3& position, const float3& eulerRotation, float height, float radius, const float4& colour);

            static void DrawArc(int numVerts, float radius, const float3& start, const float3& end, const float3& eulerRotation, const float4& colour);

            static void DrawSphere(f32 radius, const float3& position, const float4& colour);
            static void DrawCircle(int numVerts, f32 radius, const float3& position, const float3& eulerRotation, const float4& colour);
            static void DrawCone(int numCircleVerts, int numLinesToCircle, f32 angle, f32 length, const float3& position, const float3& rotation, const float4& colour);

        private:
            //Actual functions managing data parsing to save code bloat - called by public functions
            static void PopulatePointsDrawList(bool dt, const float3& pos, f32 point_radius, const float4& colour);
            static void PopulateThickLinesDrawList(bool dt, const float3& start, const float3& end, f32 line_width, const float4& colour);
            static void PopulateLinesDrawList(bool dt, const float3& start, const float3& end, const float4& colour);
        };
    }    // namespace Gfx
}    // namespace Razix
