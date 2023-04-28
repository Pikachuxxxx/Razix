// clang-format off
#include "rzxpch.h"
// clang-format on

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

namespace Razix {
    namespace Graphics {

        std::string               m_Directory;
        std::vector<RZTexture2D*> m_Textures;

        Graphics::RZTexture2D* LoadMaterialTextures(const std::string& typeName, std::vector<RZTexture2D*>& textures_loaded, const std::string& name, const std::string& directory)
        {
            for (u32 j = 0; j < textures_loaded.size(); j++) {
                if (std::strcmp(textures_loaded[j]->getPath().c_str(), (directory + "/" + name).c_str()) == 0) {
                    return textures_loaded[j];
                }
            }

            {    // If texture hasn't been loaded already, load it
                auto texture = Graphics::RZTexture2D::CreateFromFile(RZ_DEBUG_NAME_TAG_STR_F_ARG(name) directory + "/" + name, typeName, RZTexture::Wrapping::CLAMP_TO_EDGE);
                textures_loaded.push_back(texture);    // Store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.

                return texture;
            }
        }

        void RZModel::loadOBJ(const std::string& path)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            std::string       resolvedPath = path;
            tinyobj::attrib_t attrib;
            std::string       error;

            std::vector<tinyobj::shape_t>    shapes;
            std::vector<tinyobj::material_t> materials;

            m_Directory = resolvedPath.substr(0, resolvedPath.find_last_of('/'));

            std::string name = m_Directory.substr(m_Directory.find_last_of('/') + 1);

            bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &error, (resolvedPath).c_str(), (m_Directory + "/").c_str());

            if (!ok)
                RAZIX_CORE_ERROR(error);

            bool singleMesh = shapes.size() == 1;

            // for submesh in meshes
            for (const auto& shape: shapes) {
                u32       vertexCount = 0;
                const u32 numIndices  = static_cast<u32>(shape.mesh.indices.size());
                const u32 numVertices = numIndices;

                Graphics::RZVertex* vertices = new Graphics::RZVertex[numVertices];

                u32*                                        indices = new u32[numIndices];
                std::unordered_map<Graphics::RZVertex, u32> uniqueVertices;

                // for vertex in vertices in the submesh
                for (u32 i = 0; i < shape.mesh.indices.size(); i++) {
                    auto&              index = shape.mesh.indices[i];
                    Graphics::RZVertex vertex;

                    // Tex coords
                    if (!attrib.texcoords.empty()) {
                        if (index.texcoord_index > 0)
                            vertex.TexCoords = (glm::vec2(attrib.texcoords[2 * index.texcoord_index + 0], 1.0f - attrib.texcoords[2 * index.texcoord_index + 1]));
                    } else
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
                        vert_color              = glm::vec4(mp->diffuse[0], mp->diffuse[1], mp->diffuse[2], 1.0f);
                    }
                    vertex.Color = vert_color;

                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<u32>(vertexCount);
                        vertices[vertexCount]  = vertex;
                    }

                    indices[vertexCount] = uniqueVertices[vertex];

                    vertexCount++;
                }

                // Generate normal if they doesn't exist
                if (attrib.normals.empty())
                    Graphics::RZMesh::GenerateNormals(vertices, vertexCount, indices, numIndices);

                // Generate tangents
                Graphics::RZMesh::GenerateTangents(vertices, vertexCount, indices, numIndices);

                // Currently we load a pre-defined material but this should be a preset based on the current renderer and the client can select it from the list of presets
                auto shader = Graphics::RZShaderLibrary::Get().getShader("forward_renderer.rzsf");

                RZMaterial* forwardRendererMaterial = new RZMaterial(shader);

                MaterialTextures textures{};

                //if (shape.mesh.material_ids[0] >= 0) {
                //    tinyobj::material_t* mp = &materials[shape.mesh.material_ids[0]];

                //    if (mp->diffuse_texname.length() > 0) {
                //        Graphics::RZTexture2D* texture = LoadMaterialTextures("Albedo", m_Textures, mp->diffuse_texname, m_Directory);
                //        if (texture)
                //            textures.albedo = texture;
                //    }

                //    if (mp->bump_texname.length() > 0) {
                //        Graphics::RZTexture2D* texture = LoadMaterialTextures("Normal", m_Textures, mp->bump_texname, m_Directory);
                //        if (texture)
                //            textures.normal = texture;    //pbrMaterial->SetNormalMap(texture);
                //    }

                //    if (mp->roughness_texname.length() > 0) {
                //        Graphics::RZTexture2D* texture = LoadMaterialTextures("Roughness", m_Textures, mp->roughness_texname.c_str(), m_Directory);
                //        if (texture)
                //            textures.roughness = texture;
                //    }

                //    if (mp->metallic_texname.length() > 0) {
                //        Graphics::RZTexture2D* texture = LoadMaterialTextures("Metallic", m_Textures, mp->metallic_texname, m_Directory);
                //        if (texture)
                //            textures.metallic = texture;
                //    }

                //    if (mp->specular_highlight_texname.length() > 0) {
                //        Graphics::RZTexture2D* texture = LoadMaterialTextures("Specular", m_Textures, mp->specular_texname, m_Directory);
                //        if (texture)
                //            textures.specular = texture;
                //    }

                //    if (mp->emissive_texname.length() > 0) {
                //        Graphics::RZTexture2D* texture = LoadMaterialTextures("Emissive", m_Textures, mp->emissive_texname, m_Directory);
                //        if (texture)
                //            textures.emissive = texture;
                //    }
                //}

                //forwardRendererMaterial->setTextures(textures);
                forwardRendererMaterial->createDescriptorSet();

                // Create the meshes
                RZVertexBuffer* vb = RZVertexBuffer::Create(sizeof(RZVertex) * numVertices, vertices, BufferUsage::STATIC RZ_DEBUG_NAME_TAG_STR_E_ARG(name));
                // TODO: Add buffer layout from the shader
                RZVertexBufferLayout layout;
                layout.push<glm::vec3>("Position");
                layout.push<glm::vec4>("Color");
                layout.push<glm::vec2>("TexCoords");
                layout.push<glm::vec3>("Normal");
                layout.push<glm::vec3>("Tangent");
                vb->AddBufferLayout(layout);

                RZIndexBuffer* ib = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG(name) indices, numIndices);

                RZMesh* mesh = new RZMesh(vb, ib, numVertices, numIndices);
                mesh->setIndexCount(numIndices);
                mesh->setMaterial(forwardRendererMaterial);
                m_Meshes.push_back(mesh);

                delete[] vertices;
                delete[] indices;
            }
        }
    }    // namespace Graphics
}    // namespace Razix