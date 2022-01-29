#include "rzxpch.h"
#include "RZMesh.h"

#include <meshoptimizer/src/meshoptimizer.h>

namespace Razix {
    namespace Graphics {

        RZMesh::RZMesh()
            : m_VertexBuffer(nullptr), m_IndexBuffer(nullptr), m_Indices(), m_Vertices()
        {

        }

        RZMesh::RZMesh(const RZMesh& mesh)
            : m_VertexBuffer(mesh.m_VertexBuffer), m_IndexBuffer(mesh.m_IndexBuffer), m_Indices(mesh.m_Indices), m_Vertices(mesh.m_Vertices)
        {

        }

        RZMesh::RZMesh(RZVertexBuffer* vertexBuffer, RZIndexBuffer* indexBuffer, uint32_t vtxcount, uint32_t idxcount)
            : m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer), m_VertexCount(vtxcount), m_IndexCount(idxcount)
        {

        }

        RZMesh::RZMesh(const std::vector<uint32_t>& indices, const std::vector<RZVertex>& vertices, float optimiseThreshold /*= 1.0f*/)
        {
            m_Indices = indices;
            m_Vertices = vertices;

            size_t indexCount = indices.size();
            size_t target_index_count = size_t(indices.size() * optimiseThreshold);

            float target_error = 1e-3f;
            float* resultError = nullptr;

            auto newIndexCount = meshopt_simplify(m_Indices.data(), m_Indices.data(), m_Indices.size(), (const float*) (&m_Vertices[0]), m_Vertices.size(), sizeof(Graphics::RZVertex), target_index_count, target_error, resultError);

            auto newVertexCount = meshopt_optimizeVertexFetch( // return vertices (not vertex attribute values)
               (m_Vertices.data()),
               (unsigned int*) (m_Indices.data()),
               newIndexCount, // total new indices (not faces)
               (m_Vertices.data()),
               (size_t) m_Vertices.size(), // total vertices (not vertex attribute values)
               sizeof(Graphics::RZVertex) // vertex stride
            );

            RAZIX_CORE_INFO("Mesh Optimizer - Before : {0} indices {1} vertices , After : {2} indices , {3} vertices", indexCount, m_Vertices.size(), newIndexCount, newVertexCount);

            m_IndexBuffer = Graphics::RZIndexBuffer::Create(m_Indices.data(), (uint32_t) newIndexCount);
            m_VertexBuffer = Graphics::RZVertexBuffer::Create(sizeof(Graphics::RZVertex) * newVertexCount, m_Vertices.data(), BufferUsage::STATIC);
            // TODO: Add buffer layout from the shader
            RZVertexBufferLayout layout;
            layout.push<glm::vec3>("Position");
            layout.push<glm::vec4>("Color");
            layout.push<glm::vec2>("TexCoords");
            layout.push<glm::vec3>("Normal");
            layout.push<glm::vec3>("Tangent");
            m_VertexBuffer->AddBufferLayout(layout);
        }

        void RZMesh::Destroy()
        {
            m_VertexBuffer->Destroy();
            m_IndexBuffer->Destroy();
        }

    }
}