// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMesh.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/Materials/RZMaterial.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

namespace Razix {
    namespace Gfx {

        RZMesh::RZMesh()
            : m_Material(nullptr)
        {
            RZEngine::Get().GetStatistics().MeshesRendered++;
        }

#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1
        RZMesh::RZMesh(RZVertexBufferHandle vertexBuffers, RZIndexBufferHandle indexBuffer, u32 vtxcount, u32 idxcount)
            : m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer), m_VertexCount(vtxcount), m_IndexCount(idxcount)
        {
            RZEngine::Get().GetStatistics().MeshesRendered++;
        }

        RZMesh::RZMesh(const std::vector<u32>& indices, const std::vector<RZVertex>& vertices, f32 optimiseThreshold /*= 1.0f*/)
        {
            RZEngine::Get().GetStatistics().MeshesRendered++;

            auto m_Indices  = indices;
            auto m_Vertices = vertices;

            m_IndexCount  = static_cast<u32>(indices.size());
            m_VertexCount = static_cast<u32>(vertices.size());

            // Fill any missing tangents, bi tangents and normals
            //GenerateNormals(&m_Vertices[0], m_VertexCount, nullptr, 0);
            //GenerateTangentsAndBiTangents(m_Vertices.data(), m_VertexCount, nullptr, 0);
            GenerateTangents(&m_Vertices[0], m_VertexCount, nullptr, 0);

            RZBufferDesc indexBufferDesc = {};
            indexBufferDesc.name         = "IB_" + m_Name;
            indexBufferDesc.data         = m_Indices.data();
            indexBufferDesc.count        = m_IndexCount;
            indexBufferDesc.usage        = BufferUsage::Static;
            m_IndexBuffer                = RZResourceManager::Get().createIndexBuffer(indexBufferDesc);

            RZBufferDesc vertexBufferDesc = {};
            vertexBufferDesc.name         = "VB_" + m_Name;
            vertexBufferDesc.data         = (void*) vertices.data();
            vertexBufferDesc.usage        = BufferUsage::Static;
            vertexBufferDesc.size         = sizeof(Gfx::RZVertex) * static_cast<u32>(m_VertexCount);
            m_VertexBuffer                = RZResourceManager::Get().createVertexBuffer(vertexBufferDesc);

            auto vertexBufferResource = RZResourceManager::Get().getVertexBufferResource(m_VertexBuffer);
            // TODO: Add buffer layout by reflecting from the shader
            RZBufferLayout layout;
            layout.push<glm::vec3>("Position");
            layout.push<glm::vec4>("Color");
            layout.push<glm::vec2>("UV");
            layout.push<glm::vec3>("Normal");
            layout.push<glm::vec3>("Tangent");
            vertexBufferResource->AddBufferLayout(layout);

            m_Vertices.clear();
            m_Indices.clear();
        }
#endif

        RZMesh::RZMesh(const RZVertex& vertices, const std::vector<u32>& indices)
        {
            RZEngine::Get().GetStatistics().MeshesRendered++;

            // FIXME: Name is wrong here

            auto m_Indices  = indices;
            auto m_Vertices = vertices;

            m_IndexCount  = static_cast<u32>(indices.size());
            m_VertexCount = static_cast<u32>(vertices.Position.size());

            RZBufferDesc indexBufferDesc = {};
            indexBufferDesc.name         = "IB_" + m_Name;
            indexBufferDesc.data         = m_Indices.data();
            indexBufferDesc.count        = m_IndexCount;
            indexBufferDesc.usage        = BufferUsage::Static;
            m_IndexBuffer                = RZResourceManager::Get().createIndexBuffer(indexBufferDesc);

            m_Indices.clear();

            // TODO: Add buffer layout by reflecting from the shader?

            // Positions
            {
                RZBufferDesc vertexBufferDesc           = {};
                vertexBufferDesc.name                   = "VB_POSITION_" + m_Name;
                vertexBufferDesc.data                   = (void*) vertices.Position.data();
                vertexBufferDesc.usage                  = BufferUsage::Static;
                vertexBufferDesc.size                   = sizeof(glm::vec3) * static_cast<u32>(m_VertexCount);
                m_VertexBuffers[VERTEX_ATTRIBS_POS_IDX] = RZResourceManager::Get().createVertexBuffer(vertexBufferDesc);
            }

            // Colors
            {
                RZBufferDesc vertexBufferDesc           = {};
                vertexBufferDesc.name                   = "VB_COLOR_" + m_Name;
                vertexBufferDesc.data                   = (void*) vertices.Color.data();
                vertexBufferDesc.usage                  = BufferUsage::Static;
                vertexBufferDesc.size                   = sizeof(glm::vec4) * static_cast<u32>(m_VertexCount);
                m_VertexBuffers[VERTEX_ATTRIBS_COL_IDX] = RZResourceManager::Get().createVertexBuffer(vertexBufferDesc);
            }

            // UV
            {
                RZBufferDesc vertexBufferDesc          = {};
                vertexBufferDesc.name                  = "VB_TEXCOORD_" + m_Name;
                vertexBufferDesc.data                  = (void*) vertices.UV.data();
                vertexBufferDesc.usage                 = BufferUsage::Static;
                vertexBufferDesc.size                  = sizeof(glm::vec2) * static_cast<u32>(m_VertexCount);
                m_VertexBuffers[VERTEX_ATTRIBS_UV_IDX] = RZResourceManager::Get().createVertexBuffer(vertexBufferDesc);
            }

            // Normal
            {
                RZBufferDesc vertexBufferDesc           = {};
                vertexBufferDesc.name                   = "VB_NORMAL_" + m_Name;
                vertexBufferDesc.data                   = (void*) vertices.Normal.data();
                vertexBufferDesc.usage                  = BufferUsage::Static;
                vertexBufferDesc.size                   = sizeof(glm::vec3) * static_cast<u32>(m_VertexCount);
                m_VertexBuffers[VERTEX_ATTRIBS_NOR_IDX] = RZResourceManager::Get().createVertexBuffer(vertexBufferDesc);
            }

            // Tangent
            {
                RZBufferDesc vertexBufferDesc           = {};
                vertexBufferDesc.name                   = "VB_TANGENT_" + m_Name;
                vertexBufferDesc.data                   = (void*) vertices.Tangent.data();
                vertexBufferDesc.usage                  = BufferUsage::Static;
                vertexBufferDesc.size                   = sizeof(glm::vec3) * static_cast<u32>(m_VertexCount);
                m_VertexBuffers[VERTEX_ATTRIBS_TAN_IDX] = RZResourceManager::Get().createVertexBuffer(vertexBufferDesc);
            }
        }

        RZMesh::RZMesh(RZVertexBufferHandle vertexBuffer[VERTEX_ATTRIBS_COUNT], RZIndexBufferHandle indexBuffer, u32 vtxcount, u32 idxcount)
            : m_IndexBuffer(indexBuffer), m_VertexCount(vtxcount), m_IndexCount(idxcount)

        {
            RZEngine::Get().GetStatistics().MeshesRendered++;

            for (uint32_t i = 0; i < VERTEX_ATTRIBS_COUNT; i++) {
                if (vertexBuffer[i].isValid())
                    m_VertexBuffers[i] = vertexBuffer[i];
            }
        }

        RZMesh::RZMesh(const RZVertex& vertices, u32* indices, uint32_t indicesCount)
        {
            std::vector<u32> indicesVec;
            indicesVec.reserve(indicesCount);
            memcpy(indicesVec.data(), indices, indicesCount * sizeof(u32));

            *this = RZMesh(vertices, indicesVec);
        }

        void RZMesh::setName(const char* name)
        {
            m_Name = std::string(name);
        }
        void RZMesh::setName(std::string name)
        {
            m_Name = name;
        }

#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1
        void RZMesh::GenerateNormals(RZVertex* vertices, u32 vertexCount, u32* indices, u32 indexCount)
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

        void RZMesh::GenerateTangents(RZVertex* vertices, u32 vertexCount, u32* indices, u32 indexCount)
        {
            glm::vec3* tangents = new glm::vec3[vertexCount];

            for (u32 i = 0; i < vertexCount; ++i)
                tangents[i] = glm::vec3();

            if (indices) {
                for (u32 i = 0; i < indexCount; i += 3) {
                    int a = indices[i];
                    int b = indices[i + 1];
                    int c = indices[i + 2];

                    const glm::vec3 tangent = GenerateTangent(vertices[a].Position, vertices[b].Position, vertices[c].Position, vertices[a].UV, vertices[b].UV, vertices[c].UV);

                    tangents[a] += tangent;
                    tangents[b] += tangent;
                    tangents[c] += tangent;
                }
            } else {
                for (u32 i = 0; i < vertexCount; i += 3) {
                    const glm::vec3 tangent = GenerateTangent(vertices[i].Position, vertices[i + 1].Position, vertices[i + 2].Position, vertices[i].UV, vertices[i + 1].UV, vertices[i + 2].UV);

                    tangents[i] += tangent;
                    tangents[i + 1] += tangent;
                    tangents[i + 2] += tangent;
                }
            }
            for (u32 i = 0; i < vertexCount; ++i)
                vertices[i].Tangent = glm::normalize(tangents[i]);

            delete[] tangents;
        }

        void RZMesh::GenerateTangentsAndBiTangents(RZVertex* vertices, u32 vertexCount, u32* indices, u32 numIndices)
        {
            for (int i = 0; i < vertexCount; i++) {
                vertices[i].Tangent   = glm::vec3(0.0f);
                vertices[i].BiTangent = glm::vec3(0.0f);
            }

            for (uint32_t i = 0; i < numIndices; i += 3) {
                glm::vec3 v0 = vertices[indices[i]].Position;
                glm::vec3 v1 = vertices[indices[i + 1]].Position;
                glm::vec3 v2 = vertices[indices[i + 2]].Position;

                glm::vec2 uv0 = vertices[indices[i]].UV;
                glm::vec2 uv1 = vertices[indices[i + 1]].UV;
                glm::vec2 uv2 = vertices[indices[i + 2]].UV;

                glm::vec3 n0 = vertices[indices[i]].Normal;
                glm::vec3 n1 = vertices[indices[i + 1]].Normal;
                glm::vec3 n2 = vertices[indices[i + 2]].Normal;

                glm::vec3 edge1 = v1 - v0;
                glm::vec3 edge2 = v2 - v0;

                glm::vec2 deltaUV1 = uv1 - uv0;
                glm::vec2 deltaUV2 = uv2 - uv0;

                float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                glm::vec3 tangent   = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
                glm::vec3 BiTangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

                // Store tangent and BiTangent for each vertex of the triangle
                vertices[indices[i]].Tangent += tangent;
                vertices[indices[i + 1]].Tangent += tangent;
                vertices[indices[i + 2]].Tangent += tangent;

                vertices[indices[i]].BiTangent += BiTangent;
                vertices[indices[i + 1]].BiTangent += BiTangent;
                vertices[indices[i + 2]].BiTangent += BiTangent;
            }

            // Normalize the tangent and BiTangent vectors
            for (uint32_t i = 0; i < vertexCount; i++) {
                vertices[i].Tangent   = glm::normalize(vertices[i].Tangent);
                vertices[i].BiTangent = glm::normalize(vertices[i].BiTangent);
            }
        }
#endif

        void RZMesh::Destroy()
        {
            // We don't destroy the vertex and index buffers here because, the mesh might not be sole owner of the VB/IB
            // It might only be using some part of a big VB/IB so we leave it to the resource manager + RefCounter to release VB/IB
            //m_VertexBuffer->Destroy();
            //m_IndexBuffer->Destroy();
            m_Material->Destroy();
        }

        void RZMesh::bindVBsAndIB(RZDrawCommandBufferHandle cmdBuffer)
        {
            // TODO: Improve buffer layout, use reflected buffer layout to bind only those VB that have not been optimized out.

            Gfx::RZResourceManager::Get().getIndexBufferResource(m_IndexBuffer)->Bind(cmdBuffer);
            for (uint32_t i = 0; i < 1; i++) {
                auto& vertexBuffer = m_VertexBuffers[i];
                if (vertexBuffer.isValid())
                    Gfx::RZResourceManager::Get().getVertexBufferResource(vertexBuffer)->Bind(cmdBuffer);
            }
        }
    }    // namespace Graphics
}    // namespace Razix