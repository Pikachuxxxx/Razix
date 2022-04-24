#include "rzxpch.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"

#include "Razix/Graphics/API/RZTexture.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

namespace Razix {
    namespace Graphics {

        std::string m_Directory;
        std::vector<RZTexture2D*> m_Textures;

        void RZModel::loadOBJ(const std::string& path)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            std::string resolvedPath = path;
            tinyobj::attrib_t attrib;
            std::string error;

            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;

            m_Directory = resolvedPath.substr(0, resolvedPath.find_last_of('/'));

            std::string name = m_Directory.substr(m_Directory.find_last_of('/') + 1);

            bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &error, (resolvedPath).c_str(), (m_Directory + "/").c_str());

            if (!ok)
                RAZIX_CORE_ERROR(error);

            bool singleMesh = shapes.size() == 1;

            // for submesh in meshes
            for(const auto& shape : shapes){
                uint32_t vertexCount = 0;
                const uint32_t numIndices = static_cast<uint32_t>(shape.mesh.indices.size());
                const uint32_t numVertices = numIndices;

                Graphics::RZVertex* vertices = new Graphics::RZVertex[numVertices];

                uint16_t* indices = new uint16_t[numIndices];
                std::unordered_map<Graphics::RZVertex, uint32_t> uniqueVertices;

                // for vertex in vertices in the submesh
                for (uint32_t i = 0; i < shape.mesh.indices.size(); i++) {

                    auto& index = shape.mesh.indices[i];
                    Graphics::RZVertex vertex;

                    // Tex coords
                    if (!attrib.texcoords.empty())
                        vertex.TexCoords = (glm::vec2(attrib.texcoords[2 * index.texcoord_index + 0], 1.0f - attrib.texcoords[2 * index.texcoord_index + 1]));
                    else
                        vertex.TexCoords = glm::vec2(0.0f, 0.0f);

                    // Positions
                    vertex.Position = glm::vec3(attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1], attrib.vertices[3 * index.vertex_index + 2]);

                    // Normals
                    if (!attrib.normals.empty())
                        vertex.Normal = glm::vec3(attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1], attrib.normals[3 * index.normal_index + 2]);

                    // vertex colors
                    glm::vec4 vert_color = glm::vec4(0.0f);
                    if (shape.mesh.material_ids[0] >= 0) {
                         tinyobj::material_t* mp = &materials[shape.mesh.material_ids[0]];
                         vert_color = glm::vec4(mp->diffuse[0], mp->diffuse[1], mp->diffuse[2], 1.0f);
                    }
                    vertex.Color = vert_color;

                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<uint32_t>(vertexCount);
                        vertices[vertexCount] = vertex;
                    }

                    indices[vertexCount] = uniqueVertices[vertex];

                    vertexCount++;
                }

                // Generate normal if they doesn't exist
                if(attrib.normals.empty())
                    Graphics::RZMesh::GenerateNormals(vertices, vertexCount, indices, numIndices);

                // Generate tangents
                Graphics::RZMesh::GenerateTangents(vertices, vertexCount, indices, numIndices);


                // TODO: Query and retrieve the materials information, use a pre-defined shader to create the material and fill in the necessary info

                // Create the meshes
                RZVertexBuffer* vb = RZVertexBuffer::Create(sizeof(RZVertex) * numVertices, vertices, BufferUsage::STATIC, name);
                // TODO: Add buffer layout from the shader
                RZVertexBufferLayout layout;
                layout.push<glm::vec3>("Position");
                layout.push<glm::vec4>("Color");
                layout.push<glm::vec2>("TexCoords");
                layout.push<glm::vec3>("Normal");
                layout.push<glm::vec3>("Tangent");
                vb->AddBufferLayout(layout);

                RZIndexBuffer* ib = RZIndexBuffer::Create(indices, numIndices, name);

                RZMesh* mesh = new RZMesh(vb, ib, numVertices, numIndices);
                mesh->setIndexCount(numIndices);
                m_Meshes.push_back(mesh);

                delete[] vertices;
                delete[] indices;
            }
        }
    }
}