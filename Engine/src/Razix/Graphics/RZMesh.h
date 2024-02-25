#pragma once

#include "Razix/Graphics/RZVertexFormat.h"
#include "Razix/Maths/AABB.h"

namespace Razix {
    namespace Graphics {

        class RZMaterial;
        class RZVertexBuffer;
        class RZIndexBuffer;
        class RZCommandBuffer;

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
            SM_Instanced     // SMI_
        };

        constexpr cstr kStaticMeshAssetFilePrefix   = "SM_";
        constexpr cstr kDynamicMeshAssetFilePrefix  = "DM_";
        constexpr cstr kSkeletalMeshAssetFilePrefix = "SM_";

        /**
         * Mesh represents the cluster of vertex and index data which is essentially the building block of a 3D Model which will be rendered onto a scene in one draw call
         */
        class RAZIX_API RZMesh : public RZRoot
        {
        public:
            /* Creates a empty mesh */
            RZMesh();
            /* Copy constructor for Razix Mesh */
            RZMesh(const RZMesh& mesh);
            // TODO: Remove the last 2 arguments
            /**
             * Creates mesh with the given Razix vertex and Index buffer
             * 
             * @param vertexBuffer The Razix vertex buffer used to render the mesh
             * @param indexBuffer The Index Buffer used to render the mesh
             */
            RZMesh(RZVertexBuffer* vertexBuffer, RZIndexBuffer* indexBuffer, u32 vtxcount, u32 idxcount);
            /**
             * Creates a mesh with given indices and vertices
             * 
             * @param indices The index data with which the mesh will be drawn
             * @param vertices The vertices that comprises the mesh
             * optimiseThreshold The default threshold to be set for mesh optimizer to optimize the vertex data
             */
            RZMesh(const std::vector<u32>& indices, const std::vector<RZVertex>& vertices, f32 optimiseThreshold = 1.0f);

            virtual ~RZMesh() {}

            static void GenerateNormals(RZVertex* vertices, u32 vertexCount, u32* indices, u32 indexCount);
            static void GenerateTangents(RZVertex* vertices, u32 vertexCount, u32* indices, u32 indexCount);
            static void GenerateTangentsAndBiTangents(RZVertex* vertices, u32 vertexCount, u32* indices, u32 numIndices);

            void Draw(RZCommandBuffer* cmdBuf);

            void Destroy();

            RAZIX_INLINE const std::string& getName() const { return m_Name; }
            RAZIX_INLINE void               setName(const char* name);
            RAZIX_INLINE void               setName(std::string name);
            RAZIX_INLINE RZMaterial*        getMaterial() { return m_Material; }
            RAZIX_INLINE void               setMaterial(RZMaterial* mat) { m_Material = mat; }
            RAZIX_INLINE RZVertexBuffer*    getVertexBuffer() { return m_VertexBuffer; }
            RAZIX_INLINE RZIndexBuffer*     getIndexBuffer() { return m_IndexBuffer; }
            RAZIX_INLINE u32                getVerticesCount() const { return m_VertexCount; }
            RAZIX_INLINE u32                getIndexCount() const { return m_IndexCount; }
            RAZIX_INLINE void               setIndexCount(u32 count) { m_IndexCount = count; }
            RAZIX_INLINE void               setVertexCount(u32 count) { m_VertexCount = count; }
            RAZIX_INLINE std::string getPath() { return m_MeshPath; }
            RAZIX_INLINE void        setPath(std::string path) { m_MeshPath = path; }
            RAZIX_INLINE Maths::AABB getBoundingBox() { return m_BoundingBox; }
            RAZIX_INLINE glm::vec3 getMaxExtents() { return m_BoundingBox.max; }
            RAZIX_INLINE void      setMaxExtents(glm::vec3 extents) { m_BoundingBox.max = extents; }
            RAZIX_INLINE glm::vec3 getMinExtents() { return m_BoundingBox.min; }
            RAZIX_INLINE void      setMinExtents(glm::vec3 extents) { m_BoundingBox.min = extents; }
            RAZIX_INLINE u32       getBaseIndex() { return m_BaseIndex; }
            RAZIX_INLINE u32       getBaseVertex() { return m_BaseVertex; }
            RAZIX_INLINE void      setBaseIndex(u32 count) { m_BaseIndex = count; }
            RAZIX_INLINE void      setBaseVertex(u32 count) { m_BaseVertex = count; }

        private:
            std::string     m_Name = "SM_Primitive.Cube"; /* The name of the mesh                                   */
            RZMaterial*     m_Material;                   /* The material with which the mesh will be rendered with */
            RZVertexBuffer* m_VertexBuffer;               /* The Vertex Buffer that will be uploaded to the GPU     */
            RZIndexBuffer*  m_IndexBuffer;                /* The Index Buffer that will be uploaded to the GPU      */
            u32             m_IndexCount;                 /* Total indices count of the mesh                        */
            u32             m_VertexCount;                /* Total vertices count of the mesh                       */
            Maths::AABB     m_BoundingBox;
            u32             m_BaseIndex  = 0;
            u32             m_BaseVertex = 0;
            std::string     m_MeshPath;
        };

    }    // namespace Graphics
}    // namespace Razix