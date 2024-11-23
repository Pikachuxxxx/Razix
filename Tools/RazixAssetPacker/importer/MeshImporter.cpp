#include "MeshImporter.h"

#include <chrono>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <unordered_map>
#include <unordered_set>

std::string GetFilePathExtension(const std::string& FileName)
{
    auto pos = FileName.find_last_of('.');
    if (pos != std::string::npos)
        return FileName.substr(pos + 1);
    return "";
}

std::string RemoveFilePathExtension(const std::string& FileName)
{
    auto pos = FileName.find_last_of('.');
    if (pos != std::string::npos)
        return FileName.substr(0, pos);
    return FileName;
}

std::string GetFileName(const std::string& FilePath)
{
    auto pos = FilePath.find_last_of('/');
    if (pos != std::string::npos)
        return FilePath.substr(pos + 1);
    return FilePath;
}

std::string GetFileLocation(const std::string& FilePath)
{
    auto pos = FilePath.find_last_of('/');
    if (pos != std::string::npos)
        return FilePath.substr(0, pos + 1);
    return FilePath;
}

namespace Razix {
    namespace Tool {
        namespace AssetPacker {

            bool MeshImporter::importMesh(const std::string& meshFilePath, MeshImportResult& result, MeshImportOptions options)
            {
                bool isGlTF = false;

                if (meshFilePath[0] == '/' && meshFilePath[1] == '/') {
                    std::cout << "[ERROR!] Using virtual path! Please check your path and try again." << std::endl;
                    return false;
                }

                std::string extension     = GetFilePathExtension(meshFilePath);
                std::string directoryPath = GetFileLocation(meshFilePath);
                std::string meshName      = GetFileName(meshFilePath);
                meshName                  = RemoveFilePathExtension(meshName);

                if (extension == "gltf" || extension == "glb")
                    m_IsGlTF = true;

                // Let's make a bold assumption here if the model is of GLTF format it has WORLFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED in BGR order

                std::cout << "Importing Mesh...\n";

                auto start = std::chrono::high_resolution_clock::now();

                const aiScene*   scene = nullptr;
                Assimp::Importer importer;

                scene = importer.ReadFile(meshFilePath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_ImproveCacheLocality | aiProcess_JoinIdenticalVertices);

                if (scene) {
                    result.name = meshName;

                    if (scene->mRootNode->mNumChildren) {
                        // Now that the scene is loaded extract the Hierarchy for the Model
                        // Print and Store in an intermediate DS
                        printHierarchy(scene->mRootNode, scene, 0);

                        rootNode       = new Node;
                        rootNode->name = meshName.c_str();
                        extractHierarchy(rootNode, scene->mRootNode, scene, 0);
                    } else {
                        rootNode       = new Node;
                        rootNode->name = meshName.c_str();
                    }

                    result.submeshes.resize(scene->mNumMeshes);
                    result.materials.resize(scene->mNumMaterials);

                    uint32_t                 vertex_count = 0;
                    uint32_t                 index_count  = 0;
                    uint32_t                 unnamed_mats = 1;
                    std::vector<uint32_t>    temp_indices;
                    std::vector<std::string> texturePaths;

                    // Read the Materials
                    for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
                        auto& material = result.materials[i];

                        aiMaterial* assimp_material = scene->mMaterials[i];

                        // Get the name of the material
                        aiString aimat_name;
                        assimp_material->Get(AI_MATKEY_NAME, aimat_name);
                        std::string mat_name(aimat_name.C_Str());

                        std::cout << "---------------------------------------\n";
                        if (!mat_name.empty())
                            std::cout << "Loading Material... : " << mat_name << std::endl;
                        else {
                            mat_name = "Mat_" + meshName + "_" + std::to_string(i);
                            std::cout << "No Material...: " << mat_name << std::endl;
                        }

                        // Store the Name
                        strcpy(material.m_Name, mat_name.c_str());
                        // TODO: Set the Surface Type and Material Type
                        readMaterial(directoryPath, assimp_material, material);

                        std::cout << "---------------------------------------\n";
                    }

                    // Read sub Meshes Data
                    for (size_t i = 0; i < scene->mNumMeshes; i++) {
                        std::string submesh_name = scene->mMeshes[i]->mName.C_Str();

                        if (submesh_name.length() == 0)
                            submesh_name = "submesh_" + std::to_string(i);

                        strcpy(result.submeshes[i].name, submesh_name.c_str());
                        result.submeshes[i].index_count  = scene->mMeshes[i]->mNumFaces * 3;
                        result.submeshes[i].vertex_count = scene->mMeshes[i]->mNumVertices;
                        result.submeshes[i].base_index   = index_count;
                        result.submeshes[i].base_vertex  = vertex_count;

                        vertex_count += scene->mMeshes[i]->mNumVertices;
                        index_count += result.submeshes[i].index_count;

                        // Assign the material to the submesh
                        result.submeshes[i].material_index = scene->mMeshes[i]->mMaterialIndex;
                        result.submeshes[i].materialName   = result.materials[result.submeshes[i].material_index].m_Name;
                    }

                    result.vertices.setSize(vertex_count);
                    result.indices.resize(index_count);
                    temp_indices.resize(index_count);

                    aiMesh* temp_mesh;
                    int     idx          = 0;
                    int     vertex_index = 0;

                    // Create a flat hierarchy if there's not hierarchy and a the Model has a bunch of submeshes
                    if (!scene->mRootNode->mNumChildren) {
                        rootNode->numChildren = scene->mNumMeshes;
                        rootNode->children    = new Node[scene->mNumMeshes];
                    }

                    for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
                        temp_mesh = scene->mMeshes[i];

                        // Create a flat hierarchy if there's not hierarchy and a the Model has a bunch of submeshes
                        if (!scene->mRootNode->mNumChildren) {
                            rootNode->children[i].nodeType = "$MESH";
                            rootNode->children[i].name     = scene->mMeshes[i]->mName.C_Str();
                        }

                        result.submeshes[i].max_extents = glm::vec3(temp_mesh->mVertices[0].x, temp_mesh->mVertices[0].y, temp_mesh->mVertices[0].z);
                        result.submeshes[i].min_extents = glm::vec3(temp_mesh->mVertices[0].x, temp_mesh->mVertices[0].y, temp_mesh->mVertices[0].z);

                        // Read vertex data
                        //#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1
                        for (uint32_t k = 0; k < scene->mMeshes[i]->mNumVertices; k++) {
                            result.vertices.Position[vertex_index] = glm::vec3(temp_mesh->mVertices[k].x, temp_mesh->mVertices[k].y, temp_mesh->mVertices[k].z);
                            glm::vec3 n                            = glm::vec3(temp_mesh->mNormals[k].x, temp_mesh->mNormals[k].y, temp_mesh->mNormals[k].z);
                            result.vertices.Normal[vertex_index]   = n;

                            if (temp_mesh->mTangents) {
                                glm::vec3 t = glm::vec3(temp_mesh->mTangents[k].x, temp_mesh->mTangents[k].y, temp_mesh->mTangents[k].z);
                                glm::vec3 b = glm::vec3(temp_mesh->mBitangents[k].x, temp_mesh->mBitangents[k].y, temp_mesh->mBitangents[k].z);

                                // @NOTE: Assuming right handed coordinate space
                                if (glm::dot(glm::cross(n, t), b) < 0.0f)
                                    t *= -1.0f;    // Flip tangent

                                result.vertices.Tangent[vertex_index] = t;
                                //result.vertices[vertex_index].BiTangent = b;
                            }

                            if (temp_mesh->HasTextureCoords(0))
                                result.vertices.UV[vertex_index] = glm::vec2(temp_mesh->mTextureCoords[0][k].x, temp_mesh->mTextureCoords[0][k].y);

                            if (result.vertices.Position[vertex_index].x > result.submeshes[i].max_extents.x)
                                result.submeshes[i].max_extents.x = result.vertices.Position[vertex_index].x;
                            if (result.vertices.Position[vertex_index].y > result.submeshes[i].max_extents.y)
                                result.submeshes[i].max_extents.y = result.vertices.Position[vertex_index].y;
                            if (result.vertices.Position[vertex_index].z > result.submeshes[i].max_extents.z)
                                result.submeshes[i].max_extents.z = result.vertices.Position[vertex_index].z;

                            if (result.vertices.Position[vertex_index].x < result.submeshes[i].min_extents.x)
                                result.submeshes[i].min_extents.x = result.vertices.Position[vertex_index].x;
                            if (result.vertices.Position[vertex_index].y < result.submeshes[i].min_extents.y)
                                result.submeshes[i].min_extents.y = result.vertices.Position[vertex_index].y;
                            if (result.vertices.Position[vertex_index].z < result.submeshes[i].min_extents.z)
                                result.submeshes[i].min_extents.z = result.vertices.Position[vertex_index].z;

                            vertex_index++;
                        }
                        //#else
                        // Copy all 3 component stuff directly
                        //uint32_t numVerts = temp_mesh->mNumVertices;
                        //memcpy(result.vertices.Position.data(), temp_mesh->mVertices, numVerts);
                        //memcpy(result.vertices.Normal.data(), temp_mesh->mNormals, numVerts);
                        //if (temp_mesh->mTangents) {
                        //    memcpy(result.vertices.Tangent.data(), temp_mesh->mTangents, numVerts);
                        //} else
                        //    result.vertices.Tangent.resize(0);
                        //// color: 4 component
                        //if (temp_mesh->mColors[0]) {
                        //    memcpy(result.vertices.Color.data(), temp_mesh->mColors[0], numVerts);
                        //} else
                        //    result.vertices.Color.resize(0);

                        //// UV
                        //for (uint32_t k = 0; k < temp_mesh->mNumVertices; k++) {
                        //    if (temp_mesh->HasTextureCoords(0))
                        //        result.vertices.UV[vertex_index] = glm::vec2(temp_mesh->mTextureCoords[0][k].x, temp_mesh->mTextureCoords[0][k].y);

                        //    if (result.vertices.Position[vertex_index].x > result.submeshes[i].max_extents.x)
                        //        result.submeshes[i].max_extents.x = result.vertices.Position[vertex_index].x;
                        //    if (result.vertices.Position[vertex_index].y > result.submeshes[i].max_extents.y)
                        //        result.submeshes[i].max_extents.y = result.vertices.Position[vertex_index].y;
                        //    if (result.vertices.Position[vertex_index].z > result.submeshes[i].max_extents.z)
                        //        result.submeshes[i].max_extents.z = result.vertices.Position[vertex_index].z;

                        //    if (result.vertices.Position[vertex_index].x < result.submeshes[i].min_extents.x)
                        //        result.submeshes[i].min_extents.x = result.vertices.Position[vertex_index].x;
                        //    if (result.vertices.Position[vertex_index].y < result.submeshes[i].min_extents.y)
                        //        result.submeshes[i].min_extents.y = result.vertices.Position[vertex_index].y;
                        //    if (result.vertices.Position[vertex_index].z < result.submeshes[i].min_extents.z)
                        //        result.submeshes[i].min_extents.z = result.vertices.Position[vertex_index].z;

                        //    vertex_index++;
                        //}

                        //#endif
                        // Read the index data
                        for (uint32_t j = 0; j < temp_mesh->mNumFaces; j++) {
                            result.indices[idx] = temp_mesh->mFaces[j].mIndices[0];
                            idx++;
                            result.indices[idx] = temp_mesh->mFaces[j].mIndices[1];
                            idx++;
                            result.indices[idx] = temp_mesh->mFaces[j].mIndices[2];
                            idx++;
                        }
                    }

                    result.max_extents = result.submeshes[0].max_extents;
                    result.min_extents = result.submeshes[0].min_extents;

                    // Find AABB for entire result.
                    for (int i = 0; i < result.submeshes.size(); i++) {
                        if (result.submeshes[i].max_extents.x > result.max_extents.x)
                            result.max_extents.x = result.submeshes[i].max_extents.x;
                        if (result.submeshes[i].max_extents.y > result.max_extents.y)
                            result.max_extents.y = result.submeshes[i].max_extents.y;
                        if (result.submeshes[i].max_extents.z > result.max_extents.z)
                            result.max_extents.z = result.submeshes[i].max_extents.z;

                        if (result.submeshes[i].min_extents.x < result.min_extents.x)
                            result.min_extents.x = result.submeshes[i].min_extents.x;
                        if (result.submeshes[i].min_extents.y < result.min_extents.y)
                            result.min_extents.y = result.submeshes[i].min_extents.y;
                        if (result.submeshes[i].min_extents.z < result.min_extents.z)
                            result.min_extents.z = result.submeshes[i].min_extents.z;
                    }

                    auto                          finish = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time   = finish - start;

                    std::cout << "Successfully Imported mesh in " << time.count() << " seconds" << std::endl;

                } else {
                    std::cout << "[ERROR!] Failed to load model\n";
                    return false;
                }
                return true;
            }

            void MeshImporter::readMaterial(const std::string& materialsDirectory, aiMaterial* aiMat, Gfx::MaterialData& material)
            {
                // Base Color
                {
                    //material.m_MaterialProperties.workflow = (u32) Razix::Gfx::WorkFlow::WORLFLOW_LIT_PHONG;

                    bool base_color_texture_found = findTexurePath(materialsDirectory, aiMat, aiTextureType_DIFFUSE, 0, material.m_MaterialTexturePaths.albedo);
                    if (base_color_texture_found)
                        std::cout << "Diffuse Texture : " << material.m_MaterialTexturePaths.albedo << std::endl;
                    else {
                        aiColor4D base_color = aiColor4D(1.0f, 0.0f, 1.0f, 1.0f);

                        // Diffuse color, use AI_MATKEY_COLOR_TRANSPARENT for opacity later
                        aiReturn base_color_factor_found = aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, base_color);

                        material.m_MaterialProperties.albedoColor = glm::vec4(base_color.r, base_color.g, base_color.b, base_color.a);
                    }
                }

                // Roughness/Metallic
                {
                    /**
                     * If it's a GlTF we use a combined metallic roughness AO texture so 
                     */
                    if (m_IsGlTF) {
                        material.m_MaterialProperties.workflow = (u32) Razix::Gfx::WorkFlow::WORLFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED;

                        bool metal_roughness_texture_found = findTexurePath(materialsDirectory, aiMat, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, material.m_MaterialTexturePaths.metallicRoughnessAO);
                        if (metal_roughness_texture_found)
                            std::cout << "MetallicRoughness Texture : " << material.m_MaterialTexturePaths.metallic << std::endl;

                        if (!metal_roughness_texture_found) {
                            aiReturn roughness_factor_found = aiMat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, material.m_MaterialProperties.roughnessColor);
                            aiReturn metallic_factor_found  = aiMat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, material.m_MaterialProperties.metallicColor);
                            //aiReturn ao_factor_found        = aiMat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, material.m_MaterialProperties.ambientOcclusion);

                            if (roughness_factor_found == aiReturn_FAILURE)
                                material.m_MaterialProperties.roughnessColor = 0.25f;

                            if (metallic_factor_found == aiReturn_FAILURE)
                                material.m_MaterialProperties.metallicColor = 1.0f;

                            printf("Roughness: %f \n", material.m_MaterialProperties.roughnessColor);
                            printf("Metallic: %f \n", material.m_MaterialProperties.metallicColor);
                        }

                    } else {
                        material.m_MaterialProperties.workflow = (u32) Razix::Gfx::WorkFlow::WORLFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE;

                        bool roughness_texture_found = findTexurePath(materialsDirectory, aiMat, aiTextureType_SHININESS, 0, material.m_MaterialTexturePaths.roughness);
                        if (roughness_texture_found)
                            std::cout << "Roughness Texture : " << material.m_MaterialTexturePaths.roughness << std::endl;

                        if (!roughness_texture_found)
                            material.m_MaterialProperties.roughnessColor = 0.25f;

                        bool metallic_texture_found = findTexurePath(materialsDirectory, aiMat, aiTextureType_AMBIENT, 0, material.m_MaterialTexturePaths.metallic);

                        if (metallic_texture_found)
                            std::cout << "Metallic Texture : " << material.m_MaterialTexturePaths.metallic << std::endl;

                        if (!metallic_texture_found)
                            material.m_MaterialProperties.metallicColor = 1.0f;
                    }

                    // Normal
                    {
                        bool normal_texture_found = findTexurePath(materialsDirectory, aiMat, aiTextureType_NORMALS, 0, material.m_MaterialTexturePaths.normal);
                        if (normal_texture_found)
                            std::cout << "Normal Texture : " << material.m_MaterialTexturePaths.normal << std::endl;
                    }

                    // EMISSIVE
                    {
                        bool emissive_texture_found = findTexurePath(materialsDirectory, aiMat, aiTextureType_EMISSIVE, 0, material.m_MaterialTexturePaths.emissive);
                        if (emissive_texture_found)
                            std::cout << "Emissive Texture : " << material.m_MaterialTexturePaths.emissive << std::endl;
                    }
                }
            }

            bool MeshImporter::findTexurePath(const std::string& materialsDirectory, aiMaterial* aiMat, uint32_t textureType, uint32_t index, char* material)
            {
                aiString ai_path("");
                aiReturn texture_found = aiMat->GetTexture((aiTextureType) textureType, index, &ai_path);
                if (texture_found == aiReturn_FAILURE)
                    return false;

                std::string out_path = ai_path.C_Str();
                std::replace(out_path.begin(), out_path.end(), '\\', '/');

                if (out_path[0] == '.' && out_path[1] == '/')
                    out_path = out_path.substr(2, out_path.length() - 1);

                out_path = materialsDirectory + out_path;

                strcpy(material, out_path.c_str());

                return true;
            }

            void MeshImporter::printHierarchy(const aiNode* node, const aiScene* scene, uint32_t depthIndex)
            {
                if (depthIndex == 0) {
                    std::cout << "|-" << node->mName.C_Str() << " \n";
                }

                std::string indent;
                for (uint32_t j = 0; j < depthIndex; j++)
                    indent.append("  ");

                for (uint32_t i = 0; i < node->mNumChildren; i++) {
                    aiVector3D   translation, scale;
                    aiQuaternion rotation;
                    node->mChildren[i]->mTransformation.Decompose(scale, rotation, translation);

                    std::cout << indent << "|-" << node->mChildren[i]->mName.C_Str() << " \n"
                              << indent << "  Transform : (" << translation.x << ", " << translation.y << ", " << translation.z << ")" << std::endl
                              << indent << "  Type : " << (node->mChildren[i]->mNumMeshes ? "Mesh" : "Transform") << std::endl;
                    printHierarchy(node->mChildren[i], scene, ++depthIndex);
                    depthIndex--;
                }
            }

            void MeshImporter::extractHierarchy(Node* hierarchyNode, const aiNode* node, const aiScene* scene, uint32_t depthIndex)
            {
                hierarchyNode->numChildren = node->mNumChildren;
                if (hierarchyNode->numChildren) {
                    hierarchyNode->children = new Node[node->mNumChildren];

                    for (uint32_t i = 0; i < node->mNumChildren; i++) {
                        auto& child      = hierarchyNode->children[i];
                        child.name       = node->mChildren[i]->mName.C_Str();
                        const char* type = node->mChildren[i]->mNumMeshes ? "$MESH" : "$TRANSFORM";
                        child.nodeType   = type;

                        aiVector3D   translation, scale;
                        aiQuaternion rotation;
                        node->mChildren[i]->mTransformation.Decompose(scale, rotation, translation);

                        child.translation = glm::vec3(translation.x, translation.y, translation.z);
                        child.scale       = glm::vec3(scale.x, scale.y, scale.z);
                        child.rotation    = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);

                        extractHierarchy(&child, node->mChildren[i], scene, ++depthIndex);
                        depthIndex--;
                    }
                }
            }
        }    // namespace AssetPacker
    }        // namespace Tool
}    // namespace Razix
