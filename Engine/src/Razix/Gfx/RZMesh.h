#pragma once

#include "Razix/Gfx/RZVertexFormat.h"
#include "Razix/Math/AABB.h"

namespace Razix {
    namespace Gfx {

        class RZMaterial;
        class RZVertexBuffer;
        class RZIndexBuffer;
        class RZDrawCommandBuffer;

        /**
         * The submesh that will be drawn as a part of actual mesh, this gives info to draw part of the VB & IB
         */
#if 0
        struct RAZIX_API RZSubMesh
        {
            u32         vertexCount;
            u32         vertexOffset;
            u32         indexCount;
            u32         indexOffset;
            RZMaterial* material;
        };
#endif

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

        /**
         * Mesh represents the cluster of vertex and index data which is essentially the building block of a 3D Model which will be rendered onto a scene in one draw call
         */
        class RAZIX_API RZMesh
        {
        public:
            /* Creates a empty mesh */
            RZMesh();

#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1

            /**
             * Creates mesh with the given Razix vertex and Index buffer
             * 
             * @param vertexBuffer The Razix vertex buffer used to render the mesh
             * @param indexBuffer The Index Buffer used to render the mesh
             */
            RZMesh(RZVertexBufferHandle vertexBuffer, RZIndexBufferHandle indexBuffer, u32 vtxcount, u32 idxcount);
            /**
             * Creates a mesh with given indices and vertices
             * 
             * 
             * @param indices The index data with which the mesh will be drawn
             * @param vertices The vertices that comprises the mesh
             * optimiseThreshold The default threshold to be set for mesh optimizer to optimize the vertex data
             */
            RZMesh(const std::vector<u32>& indices, const std::vector<RZVertex>& vertices);
#endif

            RZMesh(RZVertexBufferHandle vertexBuffer[VERTEX_ATTRIBS_COUNT], RZIndexBufferHandle indexBuffer, u32 vtxcount, u32 idxcount);
            RZMesh(const RZVertex& vertices, const std::vector<u32>& indices);
            RZMesh(const RZVertex& vertices, u32* indices, uint32_t indicesCount);

            virtual ~RZMesh() {}

#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1
            static void GenerateNormals(RZVertex* vertices, u32 vertexCount, u32* indices, u32 indexCount);
            static void GenerateTangents(RZVertex* vertices, u32 vertexCount, u32* indices, u32 indexCount);
            static void GenerateTangentsAndBiTangents(RZVertex* vertices, u32 vertexCount, u32* indices, u32 numIndices);
#endif

            void Destroy();
            void bindVBsAndIB(RZDrawCommandBufferHandle cmdBuffer);

            inline const std::string& getName() const { return m_Name; }
            inline void               setName(const char* name) { m_Name = std::string(name); }
            inline void               setName(std::string name) { m_Name = name; }
            inline RZMaterial*        getMaterial() { return m_Material; }
            inline void               setMaterial(RZMaterial* mat) { m_Material = mat; }
#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1
            inline RZVertexBufferHandle getVertexBufferHandle() { return m_VertexBuffer; }
#else
            inline RZVertexBufferHandle getVertexBufferHandle(uint8_t attribIndex) { return m_VertexBuffers[attribIndex]; }
#endif
            inline RZIndexBufferHandle getIndexBufferHandle() { return m_IndexBuffer; }
            inline u32                 getVerticesCount() const { return m_VertexCount; }
            inline void                setVertexCount(u32 count) { m_VertexCount = count; }
            inline u32                 getIndexCount() const { return m_IndexCount; }
            inline void                setIndexCount(u32 count) { m_IndexCount = count; }
            inline std::string         getPath() { return m_MeshPath; }
            inline void                setPath(std::string path) { m_MeshPath = path; }
            inline Maths::AABB         getBoundingBox() { return m_BoundingBox; }
            inline void                setBoundingBox(Maths::AABB aabb) { m_BoundingBox = aabb; }
            inline float3              getMaxExtents() { return m_BoundingBox.max; }
            inline void                setMaxExtents(float3 extents) { m_BoundingBox.max = extents; }
            inline float3              getMinExtents() { return m_BoundingBox.min; }
            inline void                setMinExtents(float3 extents) { m_BoundingBox.min = extents; }
            inline u32                 getBaseVertex() { return m_BaseVertex; }
            inline void                setBaseVertex(u32 count) { m_BaseVertex = count; }
            inline u32                 getBaseIndex() { return m_BaseIndex; }
            inline void                setBaseIndex(u32 count) { m_BaseIndex = count; }

        private:
            std::string         m_Name        = "SM_Primitive.Cube"; /* The name of the mesh                                       */
            RZMaterial*         m_Material    = nullptr;             /* The material with which the mesh will be rendered with     */
            RZIndexBufferHandle m_IndexBuffer = {};                  /* The Index Buffer that will be uploaded to the GPU          */
            u32                 m_IndexCount  = 0;                   /* Total indices count of the mesh                            */
            u32                 m_VertexCount = 0;                   /* Total vertices count of the mesh                           */
            Maths::AABB         m_BoundingBox = {};                  /* AABB of the mesh                                           */
            u32                 m_BaseIndex   = 0;                   /* Start index of the index to use to draw the mesh           */
            u32                 m_BaseVertex  = 0;                   /* Start index of the vertex to use to draw the mesh          */
            std::string         m_MeshPath    = "";                  /* Path where the mesh is stored in disk                      */
#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1
            RZVertexBufferHandle m_VertexBuffer = {};
#else
            RZVertexBufferHandle m_VertexBuffers[VERTEX_ATTRIBS_COUNT] = {}; /* The Vertex Buffers for each vertex attrib                 */
#endif

            void initMeshFromVectors(const RZVertex& vertices, const std::vector<u32>& indices);
        };
    }    // namespace Gfx
}    // namespace Razix
