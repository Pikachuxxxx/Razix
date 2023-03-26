#pragma once

#include <glm/fwd.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace Razix {
    namespace Graphics {

        class RZMaterial;
        class RZVertexBuffer;
        class RZIndexBuffer;
        class RZCommandBuffer;

        /**
         * Simple vertex encapsulates the minimum amount of data needed for rendering a simple mesh
         */
        struct RAZIX_API RZSimpleVertex
        {
            glm::vec4 Position;
            glm::vec2 TexCoords;
        };

        /**
         * Razix Engine vertex data format that will be used to render complex 3D meshes and models 
         */
        struct RAZIX_API RZVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            glm::vec2 TexCoords;
            glm::vec3 Normal;
            glm::vec3 Tangent;

            RZVertex()
                : Position(glm::vec3(0.0f)), Color(glm::vec4(0.0f)), TexCoords(glm::vec2(0.0f)), Normal(glm::vec3(0.0f)), Tangent(glm::vec3(0.0f)) {}

            bool operator==(const RZVertex& other) const
            {
                return Position == other.Position && TexCoords == other.TexCoords && Color == other.Color && Normal == other.Normal && Tangent == other.Tangent;
            }
        };

        /**
         * The submesh that will be drawn as a part of actual mesh, this gives info to draw part of the VB & IB
         */
        struct RAZIX_API RZSubMesh
        {
            u32 vertexOffset;
            u32 indexOffset;
        };

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
            RZMesh(const std::vector<u16>& indices, const std::vector<RZVertex>& vertices, f32 optimiseThreshold = 1.0f);

            virtual ~RZMesh() {}

            static void GenerateNormals(RZVertex* vertices, u32 vertexCount, u16* indices, u32 indexCount);
            static void GenerateTangents(RZVertex* vertices, u32 vertexCount, u16* indices, u32 indexCount);

            void Draw(RZCommandBuffer* cmdBuf);

            void Destroy();

            RAZIX_INLINE const std::string& getName() const { return m_Name; }
            RAZIX_INLINE void               setName(const std::string& name) { m_Name = name; }

            RAZIX_INLINE RZMaterial* getMaterial() { return m_Material; }
            RAZIX_INLINE void        setMaterial(RZMaterial* mat) { m_Material = mat; }

            RAZIX_INLINE RZVertexBuffer* getVertexBuffer() { return m_VertexBuffer; }
            RAZIX_INLINE RZIndexBuffer*  getIndexBuffer() { return m_IndexBuffer; }
            RAZIX_FORCE_INLINE u32       getVerticesCount() const { return m_VertexCount; }
            RAZIX_FORCE_INLINE u32       getIndexCount() const { return m_IndexCount; }

            void setIndexCount(u32 count) { m_IndexCount = count; }
            void setVertexCount(u32 count) { m_VertexCount = count; }

        private:
            std::string           m_Name;         /* The name of the mesh                                   */
            RZMaterial*           m_Material;     /* The material with which the mesh will be rendered with */
            std::vector<RZVertex> m_Vertices;     /* The vertex data with which the mesh is made of         */
            std::vector<u16>      m_Indices;      /* The indices with which the mesh will be attached       */
            RZVertexBuffer*       m_VertexBuffer; /* The Vertex Buffer that will be uploaded to the GPU     */
            RZIndexBuffer*        m_IndexBuffer;  /* The Index Buffer that will be uploaded to the GPU      */
            u32                   m_IndexCount;   /* Total indices count of the mesh                        */
            u32                   m_VertexCount;  /* Total vertices count of the mesh                       */
        };

    }    // namespace Graphics
}    // namespace Razix

namespace std {
    template<>
    struct hash<Razix::Graphics::RZVertex>
    {
        sz operator()(Razix::Graphics::RZVertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.Position) ^ (hash<glm::vec2>()(vertex.TexCoords) << 1) ^ (hash<glm::vec4>()(vertex.Color) << 1) ^ (hash<glm::vec3>()(vertex.Normal) << 1) ^ (hash<glm::vec3>()(vertex.Tangent) << 1)));
        }
    };
}    // namespace std