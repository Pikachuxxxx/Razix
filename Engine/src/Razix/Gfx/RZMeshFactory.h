#pragma once

#include "Razix/Gfx/RZMesh.h"

namespace Razix {
    namespace Gfx {

        // TODO: Return RZMeshHandle once the mesh pool is threaded through here instead of a raw RZMesh value

        /* Mesh primitive describes the primitive geometry */
        enum class MeshPrimitive : int32_t
        {
            kPlane = 0,
            kCube,
            kSphere,
            kPyramid,
            kCapsule,
            kCylinder
        };

        RAZIX_API RZMesh CreatePrimitive(MeshPrimitive primitive);
    }    // namespace Gfx
}    // namespace Razix
