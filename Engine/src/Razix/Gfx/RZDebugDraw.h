#pragma once

// Modified from Source: https://github.com/jmorton06/Lumos/blob/main/Lumos/Source/Lumos/Graphics/Renderers/DebugRenderer.cpp // MIT License

#include "Razix/Core/Containers/smart_pointers.h"
#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/Utils/TRZSingleton.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Math/AABB.h"
#include "Razix/Math/Frustum.h"

namespace Razix {

    // Forward Declarations
    class RZCamera3D;

    namespace Gfx {

        // Forward Declarations
        class RZLight;

        // Why not write this file in C?
        // Using C++ because of vector types from GLM and other C++ engine types
        // Also using C++ with debug draw will allow for quick code prototyping and testing and use engine containers

        /**
         * Draws debug geometry in the scene to help with visualization and debugging
         * 
         * Uses a
         * Note: Uses Batched rendering to draw points/lines in a single draw call
         */
        class RAZIX_API RZDebugDraw final
        {
        public:
            //-------------------------------------------------------------
            static void StartUp();
            static void ShutDown();

            static void BeginDraw(const Razix::RZCamera3D* camera);
            static void IssueDrawCommands(rz_gfx_cmdbuf_handle cmdBuffer, rz_gfx_descriptor_heap_handle heap, rz_gfx_descriptor_table_handle frameData);
            static void EndDraw();

            //-------------------------------------------------------------
            // Debug Draw Public API

            //Draw Point (very small circle)
            static void DrawPoint(const float3& pos, f32 point_radius, const float3& colour);
            static void DrawPoint(const float3& pos, f32 point_radius, const float4& colour = float4(1.0f, 1.0f, 1.0f, 1.0f));

            //Draw line with thickness of 1 screen pixel regardless of distance from camera
            static void DrawLine(const float3& start, const float3& end, const float3& colour);
            static void DrawLine(const float3& start, const float3& end, const float4& colour = float4(1.0f, 1.0f, 1.0f, 1.0f));

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
        };
    }    // namespace Gfx
}    // namespace Razix
