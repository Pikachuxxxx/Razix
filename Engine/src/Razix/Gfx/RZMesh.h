#pragma once

#include "Razix/Gfx/RZVertexFormats.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Math/RZAABB.h"

#include <Razix/Asset/RZMeshAsset.h>

namespace Razix {
    namespace Gfx {

        constexpr cstr kStaticMeshAssetFilePrefix   = "SM_";
        constexpr cstr kDynamicMeshAssetFilePrefix  = "DM_";
        constexpr cstr kSkeletalMeshAssetFilePrefix = "SK_";

        struct RAZIX_API rz_mesh
        {
            RZString             name;
            rz_gfx_buffer_handle soaVertexBuffers[VERTEX_MAX_ATTRIBS_COUNT];
            rz_gfx_buffer_handle indexBuffer;
            rz_handle            material;
            u32                  vtxCount;
            u32                  idxCount;
            MeshType             meshType;
            rz_aabb              boundingBox;
            u32                  baseIndex;
            u32                  baseVertex;
        };

        struct MeshDesc
        {
            RZString             name;
            rz_gfx_buffer_handle soaVertexBuffers[VERTEX_MAX_ATTRIBS_COUNT];
            rz_gfx_buffer_handle indexBuffer;
            rz_handle            material;
            u32                  vtxCount;
            u32                  idxCount;
            MeshType             meshType;
            rz_aabb              boundingBox;
            u32                  baseIndex;
            u32                  baseVertex;
        };

        class RAZIX_API RZMesh
        {
        public:
            RZMesh() {}
            ~RZMesh() {}

            RAZIX_DEFAULT_COPYABLE_MOVABLE_CLASS(RZMesh);

            // ways to create the geometry, we can assing so it's moveable/copyable by default
            // - use the create MeshCreateDesc
            // - pass in array of vertices/index using Engine native structs using RZDynamicArray only, always use that

            RZMesh(const MeshDesc& desc);
            RZMesh(const RZVertex& vertices, const RZDynamicArray<u32> indices);

            void Destroy();

            inline const RZString&      getName() const { return m_Name; }
            inline void                 setName(const char* name) { m_Name = RZString(name); }
            inline void                 setName(RZString name) { m_Name = name; }
            inline rz_handle            getMaterial() const { return m_Material; }
            inline void                 setMaterila(rz_handle mat) { m_Material = mat; }
            inline rz_gfx_buffer_handle getVertexBufferHandle(uint8_t attribIndex) { return m_SoaVertexBuffers[attribIndex]; }
            inline rz_gfx_buffer_handle getIndexBufferHandle() { return m_IndexBuffer; }
            inline u32                  getVerticesCount() const { return m_VtxCount; }
            inline void                 setVertexCount(u32 count) { m_VtxCount = count; }
            inline u32                  getIndexCount() const { return m_IdxCount; }
            inline void                 setIndexCount(u32 count) { m_IdxCount = count; }
            inline rz_aabb              getBoundingBox() { return m_BoundingBox; }
            inline void                 setBoundingBox(rz_aabb aabb) { m_BoundingBox = aabb; }
            inline float3               getMaxExtents() { return m_BoundingBox.max; }
            inline void                 setMaxExtents(float3 extents) { m_BoundingBox.max = extents; }
            inline float3               getMinExtents() { return m_BoundingBox.min; }
            inline void                 setMinExtents(float3 extents) { m_BoundingBox.min = extents; }
            inline u32                  getBaseVertex() { return m_BaseVertex; }
            inline void                 setBaseVertex(u32 count) { m_BaseVertex = count; }
            inline u32                  getBaseIndex() { return m_BaseIndex; }
            inline void                 setBaseIndex(u32 count) { m_BaseIndex = count; }

        private:
            RZString             m_Name;
            rz_handle            m_Material;
            rz_gfx_buffer_handle m_SoaVertexBuffers[VERTEX_MAX_ATTRIBS_COUNT];
            rz_gfx_buffer_handle m_IndexBuffer;
            u32                  m_VtxCount;
            u32                  m_IdxCount;
            MeshType             m_MeshType;
            rz_aabb              m_BoundingBox;
            u32                  m_BaseIndex;
            u32                  m_BaseVertex;
            u32                  _pad0[5];

            void initMeshFromVectors(const RZVertex& vertices, const RZDynamicArray<u32>& indices);
        };
    }    // namespace Gfx
}    // namespace Razix
