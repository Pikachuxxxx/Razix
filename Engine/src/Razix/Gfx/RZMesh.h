#pragma once

#include "Razix/Gfx/RZVertexFormats.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Math/AABB.h"

namespace Razix {
    namespace Gfx {

        class RZMaterial;

        enum class MeshType
        {
            StaticMesh,      // SM_
            DynamicMesh,     // DM_
            SkeletalMesh,    // SK_
            SM_Instanced     // SM_Instanced_
        };

        constexpr cstr kStaticMeshAssetFilePrefix   = "SM_";
        constexpr cstr kDynamicMeshAssetFilePrefix  = "DM_";
        constexpr cstr kSkeletalMeshAssetFilePrefix = "SK_";

        class RAZIX_API RZMesh
        {
        public:
            RZMesh();
            RZMesh(rz_gfx_buffer_handle vertexBuffer[VERTEX_MAX_ATTRIBS_COUNT], rz_gfx_buffer_handle indexBuffer, u32 vtxcount, u32 idxcount, MeshType meshType = MeshType::StaticMesh);
            RZMesh(const RZVertex& vertices, const std::vector<u32>& indices);
            RZMesh(const RZVertex& vertices, u32* indices, uint32_t indicesCount);
            virtual ~RZMesh() {}

#ifdef MESH_UTIL_FUNCTIONS
            static void GenerateNormals(RZVertex* vertices, u32 vertexCount, u32* indices, u32 indexCount);
            static void GenerateTangents(RZVertex* vertices, u32 vertexCount, u32* indices, u32 indexCount);
            static void GenerateTangentsAndBiTangents(RZVertex* vertices, u32 vertexCount, u32* indices, u32 numIndices);
#endif
            void Destroy();

            void bindVBsAndIB(rz_gfx_cmdbuf_handle cmdBuffer);

            inline const RZString&      getName() const { return m_Name; }
            inline void                 setName(const char* name) { m_Name = RZString(name); }
            inline void                 setName(RZString name) { m_Name = name; }
            inline RZMaterial*          getMaterial() { return m_Material; }
            inline void                 setMaterial(RZMaterial* mat) { m_Material = mat; }
            inline rz_gfx_buffer_handle getVertexBufferHandle(uint8_t attribIndex) { return m_VertexBuffers[attribIndex]; }
            inline rz_gfx_buffer_handle getIndexBufferHandle() { return m_IndexBuffer; }
            inline u32                  getVerticesCount() const { return m_VertexCount; }
            inline void                 setVertexCount(u32 count) { m_VertexCount = count; }
            inline u32                  getIndexCount() const { return m_IndexCount; }
            inline void                 setIndexCount(u32 count) { m_IndexCount = count; }
            inline RZString             getPath() { return m_MeshPath; }
            inline void                 setPath(RZString path) { m_MeshPath = path; }
            inline Maths::AABB          getBoundingBox() { return m_BoundingBox; }
            inline void                 setBoundingBox(Maths::AABB aabb) { m_BoundingBox = aabb; }
            inline float3               getMaxExtents() { return m_BoundingBox.max; }
            inline void                 setMaxExtents(float3 extents) { m_BoundingBox.max = extents; }
            inline float3               getMinExtents() { return m_BoundingBox.min; }
            inline void                 setMinExtents(float3 extents) { m_BoundingBox.min = extents; }
            inline u32                  getBaseVertex() { return m_BaseVertex; }
            inline void                 setBaseVertex(u32 count) { m_BaseVertex = count; }
            inline u32                  getBaseIndex() { return m_BaseIndex; }
            inline void                 setBaseIndex(u32 count) { m_BaseIndex = count; }

        private:
            RZString             m_Name                                    = "SM_Primitive.Cube";
            RZMaterial*          m_Material                                = NULL;
            rz_gfx_buffer_handle m_IndexBuffer                             = {};
            u32                  m_IndexCount                              = 0;
            u32                  m_VertexCount                             = 0;
            Maths::AABB          m_BoundingBox                             = {};
            u32                  m_BaseIndex                               = 0;
            u32                  m_BaseVertex                              = 0;
            RZString             m_MeshPath                                = "";
            rz_gfx_buffer_handle m_VertexBuffers[VERTEX_MAX_ATTRIBS_COUNT] = {};

            void initMeshFromVectors(const RZVertex& vertices, const std::vector<u32>& indices);
        };
    }    // namespace Gfx
}    // namespace Razix
