#pragma once

#include <glm/glm.hpp>
#include <stdint.h>

namespace Razix {
    namespace AssetSystem {

#define RAZIX_ASSET_VERSION 0x1

        /* Type of the asset file */
        enum AssetType : uint8_t
        {
            ASSET_MESH     = 0,
            ASSET_MATERIAL = 1
        };

        /* Header contents of the binary asset file */
        struct BINFileHeader
        {
            char    magic[25]; /* Magic number to verify that this is a Razix Asset file indeed  */
            uint8_t version;   /* Version of the asset file                                      */
            uint8_t type;      /* Type of the asset : AssetType                                  */
        };

        struct BINMeshFileHeader
        {
            uint32_t  mesh_count;
            uint32_t  material_count;
            uint32_t  vertex_count;
            uint32_t  skeletal_vertex_count;
            uint32_t  index_count;
            glm::vec3 max_extents;
            glm::vec3 min_extents;
            char      name[250];
            uint32_t  material_index;    /* The index of the material that this submesh will use to render */
            char      materialName[250]; /* Name of the material */
            uint32_t  base_vertex;       /* vertex offset into the Vertex Buffer of the parent mesh  */
            uint32_t  base_index;        /* index offset into the index buffer of the parent mesh */
        };

    }    // namespace AssetSystem
}    // namespace Razix