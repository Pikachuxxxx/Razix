#pragma once

#include <glm/glm.hpp>
#include <stdint.h>

namespace Razix {
    namespace AssetSystem {

#define RAZIX_ASSET_VERSION_V1 0x1
#define RAZIX_ASSET_VERSION_V2 0x2
#define RAZIX_ASSET_VERSION    RAZIX_ASSET_VERSION_V2

#define MAGIC_NUM_SZ 18

        /**
         * V1: AOS
         * BINFileHeader 
         * BINMeshFileHeader
         * RZVertex Data
         * 
         * V2: SOA
         * BINFileHeader
         * BINMeshFileHeader
         * NumBlobs
         * BlobHeader (size, typeName, format)
         * positions blob
         * BlobHeader
         * UV's blob
         * BlobHeader
         * normals blob
         * BlobHeader
         * tangents blob
         * BlobHeader
         * other blobs
         */

        /* Type of the asset file */
        enum AssetType : uint8_t
        {
            ASSET_MESH     = 0,
            ASSET_MATERIAL = 1
        };

        /* Header contents of the binary asset file */
        struct BINFileHeader
        {
            char    magic[MAGIC_NUM_SZ]; /* Magic number to verify that this is a Razix Asset file indeed  */
            uint8_t version;             /* Version of the asset file                                      */
            uint8_t type;                /* Type of the asset : AssetType                                  */
        };

        struct BINMeshFileHeader
        {
            uint32_t  mesh_count;
            uint32_t  material_count;
            uint32_t  vertex_count;
            uint32_t  skeletal_vertex_count;
            uint32_t  index_count;
            uint32_t  blobs_count;
            glm::vec3 max_extents;
            glm::vec3 min_extents;
            char      name[256];
            uint32_t  material_index;    /* The index of the material that this submesh will use to render */
            char      materialName[256]; /* Name of the material */
            uint32_t  base_vertex;       /* vertex offset into the Vertex Buffer of the parent mesh  */
            uint32_t  base_index;        /* index offset into the index buffer of the parent mesh */
        };

        struct BINBlobHeader
        {
            uint32_t size;
            char     typeName[256];
            uint32_t stride;    // no need of format, unless stated
        };

    }    // namespace AssetSystem
}    // namespace Razix