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

        RAZIX_API RZMesh* CreatePrimitive(MeshPrimitive primitive);
    }    // namespace Gfx
}    // namespace Razix
