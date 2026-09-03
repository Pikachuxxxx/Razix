// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZMesh.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

namespace Razix {
    namespace Gfx {

        RZMesh::RZMesh(const MeshDesc& meshDesc)
        {
            m_IndexBuffer = meshDesc.indexBuffer;
            m_VtxCount    = meshDesc.vtxCount;
            m_IdxCount    = meshDesc.idxCount;

            u32 numAttribs = meshDesc.meshType == MeshType::kSkeletalMesh ? SKELETAL_VERTEX_ATTRIBS_COUNT : VERTEX_ATTRIBS_COUNT;
            for (uint32_t i = 0; i < numAttribs; i++) {
                if (rz_handle_is_valid(&meshDesc.soaVertexBuffers[i]))
                    m_SoaVertexBuffers[i] = meshDesc.soaVertexBuffers[i];
                else
                    RAZIX_CORE_WARN("Vertex Buffer at index {0} is not valid!", i);
            }

            RAZIX_CORE_ASSERT(rz_handle_is_valid(&meshDesc.indexBuffer), "Index buffer is invalid for this mesh: {0}", m_Name);
        }

        RZMesh::RZMesh(const RZVertex& vertices, const RZDynamicArray<u32> indices)
        {
            initMeshFromVectors(vertices, indices);
        }

        // Old AoS-vertex utils, incompatible with the current SoA RZVertex layout (RZDynamicArray<float3> Position, etc).
        // Needs a rewrite against the SoA layout before it can be re-enabled.
#if 0
        RZMesh::RZMesh(const RZVertex& vertices, u32* indices, uint32_t indicesCount)
        {
            RZDynamicArray<u32> indicesVec;
            indicesVec.resize(indicesCount);
            memcpy(indicesVec.data(), indices, indicesCount * sizeof(u32));

            initMeshFromVectors(vertices, indicesVec);
        }

        void RZMesh::GenerateNormals(RZVertex* vertices, u32 vertexCount, u32* indices, u32 indexCount)
        {
            float3* normals = new float3[vertexCount];

            for (u32 i = 0; i < vertexCount; ++i)
                normals[i] = float3();

            if (indices) {
                for (u32 i = 0; i < indexCount; i += 3) {
                    const int a = indices[i];
                    const int b = indices[i + 1];
                    const int c = indices[i + 2];

                    const float3 _normal = cross((vertices[b].Position - vertices[a].Position), (vertices[c].Position - vertices[a].Position));

                    normals[a] += _normal;
                    normals[b] += _normal;
                    normals[c] += _normal;
                }
            } else {
                // It's just a list of triangles, so generate face normals
                for (u32 i = 0; i < vertexCount; i += 3) {
                    float3& a = vertices[i].Position;
                    float3& b = vertices[i + 1].Position;
                    float3& c = vertices[i + 2].Position;

                    const float3 _normal = cross(b - a, c - a);

                    normals[i]     = _normal;
                    normals[i + 1] = _normal;
                    normals[i + 2] = _normal;
                }
            }

            for (u32 i = 0; i < vertexCount; ++i)
                vertices[i].Normal = normalize(normals[i]);

            delete[] normals;
        }

        static float3 GenerateTangent(const float3& a, const float3& b, const float3& c, const float2& ta, const float2& tb, const float2& tc)
        {
            const float2 coord1 = tb - ta;
            const float2 coord2 = tc - ta;

            const float3 vertex1 = b - a;
            const float3 vertex2 = c - a;

            const float3 axis = float3(vertex1 * coord2.y - vertex2 * coord1.y);

            const f32 factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);

            return axis * factor;
        }

        void RZMesh::GenerateTangents(RZVertex* vertices, u32 vertexCount, u32* indices, u32 indexCount)
        {
            float3* tangents = new float3[vertexCount];

            for (u32 i = 0; i < vertexCount; ++i)
                tangents[i] = float3();

            if (indices) {
                for (u32 i = 0; i < indexCount; i += 3) {
                    int a = indices[i];
                    int b = indices[i + 1];
                    int c = indices[i + 2];

                    const float3 tangent = GenerateTangent(vertices[a].Position, vertices[b].Position, vertices[c].Position, vertices[a].UV, vertices[b].UV, vertices[c].UV);

                    tangents[a] += tangent;
                    tangents[b] += tangent;
                    tangents[c] += tangent;
                }
            } else {
                for (u32 i = 0; i < vertexCount; i += 3) {
                    const float3 tangent = GenerateTangent(vertices[i].Position, vertices[i + 1].Position, vertices[i + 2].Position, vertices[i].UV, vertices[i + 1].UV, vertices[i + 2].UV);

                    tangents[i] += tangent;
                    tangents[i + 1] += tangent;
                    tangents[i + 2] += tangent;
                }
            }
            for (u32 i = 0; i < vertexCount; ++i)
                vertices[i].Tangent = normalize(tangents[i]);

            delete[] tangents;
        }

        void RZMesh::GenerateTangentsAndBiTangents(RZVertex* vertices, u32 vertexCount, u32* indices, u32 numIndices)
        {
            for (int i = 0; i < vertexCount; i++) {
                vertices[i].Tangent   = float3(0.0f);
                vertices[i].BiTangent = float3(0.0f);
            }

            for (uint32_t i = 0; i < numIndices; i += 3) {
                float3 v0 = vertices[indices[i]].Position;
                float3 v1 = vertices[indices[i + 1]].Position;
                float3 v2 = vertices[indices[i + 2]].Position;

                float2 uv0 = vertices[indices[i]].UV;
                float2 uv1 = vertices[indices[i + 1]].UV;
                float2 uv2 = vertices[indices[i + 2]].UV;

                float3 n0 = vertices[indices[i]].Normal;
                float3 n1 = vertices[indices[i + 1]].Normal;
                float3 n2 = vertices[indices[i + 2]].Normal;

                float3 edge1 = v1 - v0;
                float3 edge2 = v2 - v0;

                float2 deltaUV1 = uv1 - uv0;
                float2 deltaUV2 = uv2 - uv0;

                float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                float3 tangent   = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
                float3 BiTangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

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
                vertices[i].Tangent   = normalize(vertices[i].Tangent);
                vertices[i].BiTangent = normalize(vertices[i].BiTangent);
            }
        }
#endif

        void RZMesh::Destroy()
        {
            // We don't destroy the material here, m_Material is just a handle, ownership lives with the AssetDB/ResourceManager
            for (uint32_t i = 0; i < VERTEX_MAX_ATTRIBS_COUNT; i++) {
                if (rz_handle_is_valid(&m_SoaVertexBuffers[i]))
                    RZResourceManager::Get().destroyBuffer(m_SoaVertexBuffers[i]);
            }
            RZResourceManager::Get().destroyBuffer(m_IndexBuffer);
        }

        void RZMesh::initMeshFromVectors(const RZVertex& vertices, const RZDynamicArray<u32>& indices)
        {
            RZDynamicArray<u32> meshIndices = indices;

            m_IdxCount = static_cast<u32>(indices.size());
            m_VtxCount = static_cast<u32>(vertices.Position.size());

            rz_gfx_buffer_desc indexBufferDesc = {};
            indexBufferDesc.type               = RZ_GFX_BUFFER_TYPE_INDEX;
            indexBufferDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
            indexBufferDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_NONE;
            indexBufferDesc.sizeInBytes        = static_cast<u32>(meshIndices.size()) * sizeof(u32);
            indexBufferDesc.pInitData          = meshIndices.data();
            m_IndexBuffer                      = RZResourceManager::Get().createBuffer(("IB_" + m_Name).c_str(), indexBufferDesc);

            meshIndices.clear();

            // TODO: Add buffer layout by reflecting from the shader?

            // Positions
            {
                rz_gfx_buffer_desc vertexBufferDesc                = {};
                vertexBufferDesc.type                              = RZ_GFX_BUFFER_TYPE_VERTEX;
                vertexBufferDesc.usage                             = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
                vertexBufferDesc.resourceHints                     = RZ_GFX_RESOURCE_VIEW_FLAG_NONE;
                vertexBufferDesc.sizeInBytes                       = sizeof(float3) * m_VtxCount;
                vertexBufferDesc.pInitData                         = vertices.Position.data();
                m_SoaVertexBuffers[VERTEX_ATTRIBS_POS_IDX] = RZResourceManager::Get().createBuffer(("VB_POSITION_" + m_Name).c_str(), vertexBufferDesc);
            }

            // Colors
            {
                rz_gfx_buffer_desc vertexBufferDesc                = {};
                vertexBufferDesc.type                              = RZ_GFX_BUFFER_TYPE_VERTEX;
                vertexBufferDesc.usage                             = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
                vertexBufferDesc.resourceHints                     = RZ_GFX_RESOURCE_VIEW_FLAG_NONE;
                vertexBufferDesc.sizeInBytes                       = sizeof(float4) * m_VtxCount;
                vertexBufferDesc.pInitData                         = vertices.Color.data();
                m_SoaVertexBuffers[VERTEX_ATTRIBS_COL_IDX] = RZResourceManager::Get().createBuffer(("VB_COLOR_" + m_Name).c_str(), vertexBufferDesc);
            }

            // UV
            {
                rz_gfx_buffer_desc vertexBufferDesc               = {};
                vertexBufferDesc.type                             = RZ_GFX_BUFFER_TYPE_VERTEX;
                vertexBufferDesc.usage                            = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
                vertexBufferDesc.resourceHints                    = RZ_GFX_RESOURCE_VIEW_FLAG_NONE;
                vertexBufferDesc.sizeInBytes                      = sizeof(float2) * m_VtxCount;
                vertexBufferDesc.pInitData                        = vertices.UV.data();
                m_SoaVertexBuffers[VERTEX_ATTRIBS_UV_IDX] = RZResourceManager::Get().createBuffer(("VB_TEXCOORD_" + m_Name).c_str(), vertexBufferDesc);
            }

            // Normal
            {
                rz_gfx_buffer_desc vertexBufferDesc                = {};
                vertexBufferDesc.type                              = RZ_GFX_BUFFER_TYPE_VERTEX;
                vertexBufferDesc.usage                             = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
                vertexBufferDesc.resourceHints                     = RZ_GFX_RESOURCE_VIEW_FLAG_NONE;
                vertexBufferDesc.sizeInBytes                       = sizeof(float3) * m_VtxCount;
                vertexBufferDesc.pInitData                         = vertices.Normal.data();
                m_SoaVertexBuffers[VERTEX_ATTRIBS_NOR_IDX] = RZResourceManager::Get().createBuffer(("VB_NORMAL_" + m_Name).c_str(), vertexBufferDesc);
            }

            // Tangent
            {
                rz_gfx_buffer_desc vertexBufferDesc                = {};
                vertexBufferDesc.type                              = RZ_GFX_BUFFER_TYPE_VERTEX;
                vertexBufferDesc.usage                             = RZ_GFX_BUFFER_USAGE_TYPE_STATIC;
                vertexBufferDesc.resourceHints                     = RZ_GFX_RESOURCE_VIEW_FLAG_NONE;
                vertexBufferDesc.sizeInBytes                       = sizeof(float3) * m_VtxCount;
                vertexBufferDesc.pInitData                         = vertices.Tangent.data();
                m_SoaVertexBuffers[VERTEX_ATTRIBS_TAN_IDX] = RZResourceManager::Get().createBuffer(("VB_TANGENT_" + m_Name).c_str(), vertexBufferDesc);
            }
        }
    }    // namespace Gfx
}    // namespace Razix
