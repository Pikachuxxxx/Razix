#pragma once

// Based on https://github.com/diharaw/asset-core

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "Razix/AssetSystem/RZAssetFileSpec.h"

#include "Razix/Gfx/Materials/RZMaterialData.h"
#include "Razix/Gfx/RZVertexFormat.h"

namespace Razix {
    namespace Gfx {
        class RZMaterial;
    }
}    // namespace Razix

namespace Razix {
    namespace Tool {
        namespace AssetPacker {

            /**
             * Submesh is a part of mesh that will be drawn, can be used to split a mesh into multiple small sub-meshes
             * Or it can be used to create multiple materials for a mesh. [Design TBD]
             */
            struct SubMesh
            {
                uint32_t    material_index; /* The index of the material that this submesh will use to render */
                std::string materialName;   /* Name of the material */
                uint32_t    index_count;    /* Total indices count in the sub mesh */
                uint32_t    vertex_count;   /* Total vertices count in the sub mesh */
                uint32_t    base_vertex;    /* vertex offset into the Vertex Buffer of the parent mesh  */
                uint32_t    base_index;     /* index offset into the index buffer of the parent mesh */
                glm::vec3   max_extents;    /* Maximum extents of the sub mesh */
                glm::vec3   min_extents;    /* Minimum extents of the sub mesh */
                char        name[150];      /* Name of the sub-mesh */
            };

            //--------------------------------------------------------------------------------
            // Mesh Import Result
            //--------------------------------------------------------------------------------

            struct MeshImportResult
            {
                std::string                         name;
                Razix::Gfx::RZVertex           vertices;
                Razix::Gfx::RZSkeletalVertex   skeletal_vertices;
                std::vector<uint32_t>               indices;
                std::vector<SubMesh>                submeshes;
                std::vector<Gfx::MaterialData> materials;
                glm::vec3                           max_extents;
                glm::vec3                           min_extents;
            };

            //--------------------------------------------------------------------------------
            // Hierarchy
            //--------------------------------------------------------------------------------

            // TODO: Export this to a .rzmodel file for hierarchy info
            struct Node
            {
                Node*     children    = nullptr;
                uint32_t  numChildren = 0;
                glm::vec3 translation = glm::vec3(0.0f);
                glm::quat rotation /*= glm::quat(0.0f)*/;
                glm::vec3 scale = glm::vec3(1.0f);
                //char      name[250];
                //char*     nodeType;
                std::string name;
                std::string nodeType;    // $MESH, $TRANSFORM, $MATERIAL
            };

        }    // namespace AssetPacker
    }        // namespace Tool
}    // namespace Razix