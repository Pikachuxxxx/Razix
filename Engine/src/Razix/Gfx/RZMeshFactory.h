#pragma once

#include <glm/fwd.hpp>

namespace Razix {
    namespace Gfx {

        // TODO: Use RZMeshHandle
        class RZMesh;

        /* Mesh primitive describes the primitive geometry */
        enum MeshPrimitive : int32_t
        {
            Plane = 0,
            Cube,
            Sphere,
            Pyramid,    // WIP
            Capsule,    // WIP
            Cylinder    // WIP
        };

        namespace MeshFactory {

            /**
             * Mesh factory generates various mesh geometry primitives
             */

            RAZIX_API RZMesh* CreatePrimitive(MeshPrimitive primitive);

            RZMesh* CreatePlane(f32 width = 10.0f, f32 height = 10.0f, const float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f));
            RZMesh* CreateCube();
            RZMesh* CreatePyramid();
            RZMesh* CreateSphere(u32 xSegments = 64, u32 ySegments = 64);
            RZMesh* CreateIcoSphere(u32 radius = 1, u32 subdivision = 64);
            RZMesh* CreateCapsule(f32 radius = 1.0f, f32 midHeight = 1.0f, int radialSegments = 64, int rings = 8);
            RZMesh* CreateCylinder(f32 bottomRadius = 1.0f, f32 topRadius = 1.0f, f32 height = 1.0f, int radialSegments = 64, int rings = 8);
        }    // namespace MeshFactory
    }    // namespace Gfx
}    // namespace Razix
