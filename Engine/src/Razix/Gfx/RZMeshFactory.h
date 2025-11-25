#pragma once

#include <glm/fwd.hpp>

namespace Razix {
    namespace Gfx {

        // TODO: Use RZMeshHandle
        class RZMesh;

        /* Mesh primitive describes the primitive geometry */
        enum class MeshPrimitive : int32_t
        {
            kPlane = 0,
            kCube,
            kSphere,
            kPyramid,    // WIP
            kCapsule,    // WIP
            kCylinder    // WIP
        };

        RAZIX_API RZMesh* CreatePrimitive(MeshPrimitive primitive);
    }    // namespace Gfx
}    // namespace Razix
