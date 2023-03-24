#pragma once

#include <glm/fwd.hpp>

namespace Razix {
    namespace Graphics {

        class RZMesh;

        /* Mesh primitive describes the primitive geometry */
        enum MeshPrimitive : int32_t
        {
            Plane      = 0,
            ScreenQuad = 1,
            Cube       = 2,
            Pyramid    = 3,
            Sphere     = 4,
            Capsule    = 5,
            Cylinder   = 6
        };

        namespace MeshFactory {

            /**
             * Mesh factory generates various mesh geometry primitives
             */

            RAZIX_API RZMesh* CreatePrimitive(MeshPrimitive primitive);

            RZMesh* CreateScreenQuad();
            RZMesh* CreatePlane(f32 width = 10.0f, f32 height = 10.0f, const glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            RZMesh* CreateCube();
            RZMesh* CreatePyramid();
            RZMesh* CreateSphere(u32 xSegments = 64, u32 ySegments = 64);
            RZMesh* CreateIcoSphere(u32 radius = 1, u32 subdivision = 64);
            RZMesh* CreateCapsule(f32 radius = 1.0f, f32 midHeight = 1.0f, int radialSegments = 64, int rings = 8);
            RZMesh* CreateCylinder(f32 bottomRadius = 1.0f, f32 topRadius = 1.0f, f32 height = 1.0f, int radialSegments = 64, int rings = 8);
        }    // namespace MeshFactory
    }        // namespace Graphics
}    // namespace Razix
