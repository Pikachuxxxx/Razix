#pragma once

#include "Razix/Graphics/RZVertexFormat.h"

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

            void Draw(RZCommandBuffer* cmdBuf);

            void Destroy();

            RAZIX_INLINE const std::string& getName() const { return m_Name; }
            RAZIX_INLINE void               setName(const char* name) { m_Name = std::string(name); }
            RAZIX_INLINE void               setName(std::string name) { strcpy(&m_Name[0], name.c_str()); }

            RAZIX_INLINE RZMaterial* getMaterial() { return m_Material; }
            RAZIX_INLINE void        setMaterial(RZMaterial* mat) { m_Material = mat; }

            RAZIX_INLINE RZVertexBuffer* getVertexBuffer() { return m_VertexBuffer; }
            RAZIX_INLINE RZIndexBuffer*  getIndexBuffer() { return m_IndexBuffer; }
            RAZIX_FORCE_INLINE u32       getVerticesCount() const { return m_VertexCount; }
            RAZIX_FORCE_INLINE u32       getIndexCount() const { return m_IndexCount; }

            void setIndexCount(u32 count) { m_IndexCount = count; }
            void setVertexCount(u32 count) { m_VertexCount = count; }

            RAZIX_INLINE glm::vec3 getMaxExtents() { return m_MaxExtents; }
            RAZIX_INLINE void      setMaxExtents(glm::vec3 extents) { m_MaxExtents = extents; }
            RAZIX_INLINE glm::vec3 getMinExtents() { return m_MinExtents; }
            RAZIX_INLINE void      setMinExtents(glm::vec3 extents) { m_MinExtents = extents; }

            RAZIX_INLINE glm::vec3 getModelMaxExtents() { return m_ModelMaxExtents; }
            RAZIX_INLINE void      setModelMaxExtents(glm::vec3 extents) { m_ModelMaxExtents = extents; }
            RAZIX_INLINE glm::vec3 getModelMinExtents() { return m_ModelMinExtents; }
            RAZIX_INLINE void      setModelMinExtents(glm::vec3 extents) { m_ModelMinExtents = extents; }

            u32  getBaseIndex() { return m_BaseIndex; }
            u32  getBaseVertex() { return m_BaseVertex; }
            void setBaseIndex(u32 count) { m_BaseIndex = count; }
            void setBaseVertex(u32 count) { m_BaseVertex = count; }

            std::string getPath() { return m_MeshPath; }
            void        setPath(std::string path) { m_MeshPath = path; }

        private:
            std::string           m_Name;         /* The name of the mesh                                   */
            RZMaterial*           m_Material;     /* The material with which the mesh will be rendered with */
            std::vector<RZVertex> m_Vertices;     /* The vertex data with which the mesh is made of         */
            std::vector<u32>      m_Indices;      /* The indices with which the mesh will be attached       */
            RZVertexBuffer*       m_VertexBuffer; /* The Vertex Buffer that will be uploaded to the GPU     */
            RZIndexBuffer*        m_IndexBuffer;  /* The Index Buffer that will be uploaded to the GPU      */
            u32                   m_IndexCount;   /* Total indices count of the mesh                        */
            u32                   m_VertexCount;  /* Total vertices count of the mesh                       */
            glm::vec3             m_MinExtents;
            glm::vec3             m_MaxExtents;
            glm::vec3             m_ModelMaxExtents;
            glm::vec3             m_ModelMinExtents;
            u32                   m_BaseIndex  = 0;
            u32                   m_BaseVertex = 0;
            std::string           m_MeshPath;
        };

    }    // namespace Graphics
}    // namespace Razix

namespace std {
    template<>
    struct hash<Razix::Graphics::RZVertex>
    {
        sz operator()(Razix::Graphics::RZVertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.Position) ^ (hash<glm::vec2>()(vertex.UV) << 1) ^ (hash<glm::vec4>()(vertex.Color) << 1) ^ (hash<glm::vec3>()(vertex.Normal) << 1) ^ (hash<glm::vec3>()(vertex.Tangent) << 1)));
        }
    };
}    // namespace std