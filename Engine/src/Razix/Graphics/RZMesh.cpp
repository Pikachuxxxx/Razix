// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMesh.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"
#include "Razix/Graphics/Materials/RZMaterial.h"

#include <meshoptimizer/src/meshoptimizer.h>

namespace Razix {
    namespace Graphics {

        RZMesh::RZMesh()
            : m_VertexBuffer(nullptr), m_IndexBuffer(nullptr), m_Indices(), m_Vertices(), m_Material(nullptr)
        {
            RZEngine::Get().GetStatistics().MeshesRendered++;
        }

        RZMesh::RZMesh(const RZMesh& mesh)
            : m_VertexBuffer(mesh.m_VertexBuffer), m_IndexBuffer(mesh.m_IndexBuffer), m_Indices(mesh.m_Indices), m_Vertices(mesh.m_Vertices), m_Material(mesh.m_Material)
        {
            RZEngine::Get().GetStatistics().MeshesRendered++;
        }

        RZMesh::RZMesh(RZVertexBuffer* vertexBuffer, RZIndexBuffer* indexBuffer, u32 vtxcount, u32 idxcount)
            : m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer), m_VertexCount(vtxcount), m_IndexCount(idxcount)
        {
            RZEngine::Get().GetStatistics().MeshesRendered++;
        }

        RZMesh::RZMesh(const std::vector<u16>& indices, const std::vector<RZVertex>& vertices, f32 optimiseThreshold /*= 1.0f*/)
        {
            RZEngine::Get().GetStatistics().MeshesRendered++;

            m_Indices  = indices;
            m_Vertices = vertices;

            sz indexCount         = indices.size();
            sz target_index_count = sz(indices.size() * optimiseThreshold);

            m_IndexCount  = static_cast<u32>(indices.size());
            m_VertexCount = static_cast<u32>(vertices.size());

            f32  target_error = 1e-3f;
            f32* resultError  = nullptr;

            auto newIndexCount = meshopt_simplify(m_Indices.data(), m_Indices.data(), m_Indices.size(), (const f32*) (&m_Vertices[0]), m_Vertices.size(), sizeof(Graphics::RZVertex), target_index_count, target_error, resultError);

            auto newVertexCount = meshopt_optimizeVertexFetch(    // return vertices (not vertex attribute values)
                (m_Vertices.data()),
                (unsigned short*) (m_Indices.data()),
                newIndexCount,    // total new indices (not faces)
                (m_Vertices.data()),
                (sz) m_Vertices.size(),    // total vertices (not vertex attribute values)
                sizeof(Graphics::RZVertex)     // vertex stride
            );

            RAZIX_CORE_INFO("Mesh Optimizer - Before : {0} indices {1} vertices , After : {2} indices , {3} vertices", indexCount, m_Vertices.size(), newIndexCount, newVertexCount);

            m_IndexBuffer  = Graphics::RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG(m_Name) m_Indices.data(), (u16) newIndexCount);
            m_VertexBuffer = Graphics::RZVertexBuffer::Create(sizeof(Graphics::RZVertex) * static_cast<u32>(newVertexCount), m_Vertices.data(), BufferUsage::STATIC RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Name));
            // TODO: Add buffer layout by reflecting from the shader
            RZVertexBufferLayout layout;
            layout.push<glm::vec3>("Position");
            layout.push<glm::vec4>("Color");
            layout.push<glm::vec2>("TexCoords");
            layout.push<glm::vec3>("Normal");
            layout.push<glm::vec3>("Tangent");
            m_VertexBuffer->AddBufferLayout(layout);
        }

        void RZMesh::GenerateNormals(RZVertex* vertices, u32 vertexCount, u16* indices, u32 indexCount)
        {
            glm::vec3* normals = new glm::vec3[vertexCount];

            for (u32 i = 0; i < vertexCount; ++i)
                normals[i] = glm::vec3();

            if (indices) {
                for (u32 i = 0; i < indexCount; i += 3) {
                    const int a = indices[i];
                    const int b = indices[i + 1];
                    const int c = indices[i + 2];

                    const glm::vec3 _normal = glm::cross((vertices[b].Position - vertices[a].Position), (vertices[c].Position - vertices[a].Position));

                    normals[a] += _normal;
                    normals[b] += _normal;
                    normals[c] += _normal;
                }
            } else {
                // It's just a list of triangles, so generate face normals
                for (u32 i = 0; i < vertexCount; i += 3) {
                    glm::vec3& a = vertices[i].Position;
                    glm::vec3& b = vertices[i + 1].Position;
                    glm::vec3& c = vertices[i + 2].Position;

                    const glm::vec3 _normal = glm::cross(b - a, c - a);

                    normals[i]     = _normal;
                    normals[i + 1] = _normal;
                    normals[i + 2] = _normal;
                }
            }

            for (u32 i = 0; i < vertexCount; ++i)
                vertices[i].Normal = glm::normalize(normals[i]);

            delete[] normals;
        }

        static glm::vec3 GenerateTangent(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec2& ta, const glm::vec2& tb, const glm::vec2& tc)
        {
            const glm::vec2 coord1 = tb - ta;
            const glm::vec2 coord2 = tc - ta;

            const glm::vec3 vertex1 = b - a;
            const glm::vec3 vertex2 = c - a;

            const glm::vec3 axis = glm::vec3(vertex1 * coord2.y - vertex2 * coord1.y);

            const f32 factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);

            return axis * factor;
        }

        void RZMesh::GenerateTangents(RZVertex* vertices, u32 vertexCount, u16* indices, u32 indexCount)
        {
            glm::vec3* tangents = new glm::vec3[vertexCount];

            for (u32 i = 0; i < vertexCount; ++i)
                tangents[i] = glm::vec3();

            if (indices) {
                for (u32 i = 0; i < indexCount; i += 3) {
                    int a = indices[i];
                    int b = indices[i + 1];
                    int c = indices[i + 2];

                    const glm::vec3 tangent = GenerateTangent(vertices[a].Position, vertices[b].Position, vertices[c].Position, vertices[a].TexCoords, vertices[b].TexCoords, vertices[c].TexCoords);

                    tangents[a] += tangent;
                    tangents[b] += tangent;
                    tangents[c] += tangent;
                }
            } else {
                for (u32 i = 0; i < vertexCount; i += 3) {
                    const glm::vec3 tangent = GenerateTangent(vertices[i].Position, vertices[i + 1].Position, vertices[i + 2].Position, vertices[i].TexCoords, vertices[i + 1].TexCoords, vertices[i + 2].TexCoords);

                    tangents[i] += tangent;
                    tangents[i + 1] += tangent;
                    tangents[i + 2] += tangent;
                }
            }
            for (u32 i = 0; i < vertexCount; ++i)
                vertices[i].Tangent = glm::normalize(tangents[i]);

            delete[] tangents;
        }

        void RZMesh::Draw(RZCommandBuffer* cmdBuf)
        {
            m_VertexBuffer->Bind(cmdBuf);
            m_IndexBuffer->Bind(cmdBuf);

            m_Material->Bind();

            RHI::DrawIndexed(cmdBuf, m_IndexCount, 1, 0, 0, 0);
        }

        void RZMesh::Destroy()
        {
            m_VertexBuffer->Destroy();
            m_IndexBuffer->Destroy();
            m_Material->Destroy();
        }
    }    // namespace Graphics
}    // namespace Razix