#pragma once

#include "Razix/Core/RZDataTypes.h"

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
            char magic[MAGIC_NUM_SZ]; /* Magic number to verify that this is a Razix Asset file indeed  */
            u8   version;             /* Version of the asset file                                      */
            u8   type;                /* Type of the asset : AssetType                                  */
        };

        struct BINMeshFileHeader
        {
            u32    mesh_count;
            u32    material_count;
            u32    vertex_count;
            u32    skeletal_vertex_count;
            u32    index_count;
            u32    blobs_count;
            float3 max_extents;
            float3 min_extents;
            char   name[256];
            u32    material_index;    /* The index of the material that this submesh will use to render */
            char   materialName[256]; /* Name of the material */
            u32    base_vertex;       /* vertex offset into the Vertex Buffer of the parent mesh  */
            u32    base_index;        /* index offset into the index buffer of the parent mesh */
        };

        struct BINBlobHeader
        {
            u32  size;
            char typeName[256];
            u32  stride;    // no need of format, unless stated
        };

    }    // namespace AssetSystem
}    // namespace Razix