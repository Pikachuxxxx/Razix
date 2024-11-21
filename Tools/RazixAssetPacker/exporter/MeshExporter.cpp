#include "MeshExporter.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "Razix/AssetSystem/RZAssetFileSpec.h"

#include <assimp/material.h>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

#include <filesystem>

using namespace Razix::AssetSystem;

#define WRITE_AND_OFFSET(stream, dest, size, offset) \
    stream.write((char*) dest, size);                \
    offset += size;                                  \
    stream.seekg(offset);

namespace Razix {
    namespace Tool {
        namespace AssetPacker {

            bool MeshExporter::exportMesh(const MeshImportResult& import_result, const MeshExportOptions& options)
            {
                auto start = std::chrono::high_resolution_clock::now();

                // Create a directory in the name of the model scene

                std::string materials_path = options.assetsOutputDirectory + "Materials/" + import_result.name + "/";
                std::filesystem::create_directory(materials_path);
                std::string mesh_path = options.assetsOutputDirectory + "/Cache/Meshes/" + import_result.name + "/";
                std::filesystem::create_directory(mesh_path);

                for (size_t i = 0; i < import_result.submeshes.size(); i++) {
                    const auto submesh = import_result.submeshes[i];

                    std::string export_path = mesh_path + import_result.name + "_" + submesh.name + ".rzmesh";

                    // Don't export if file exists
                    bool exist = std::filesystem::exists(export_path);
                    if (exist)
                        continue;

                    // Export the Mesh
                    std::fstream f(export_path, std::ios::out | std::ios::binary);

                    if (f.is_open()) {
                        BINFileHeader fh{};
                        char*         magic = new char[18];

                        magic[0]  = 'r';
                        magic[1]  = 'a';
                        magic[2]  = 'z';
                        magic[3]  = 'i';
                        magic[4]  = 'x';
                        magic[5]  = '_';
                        magic[6]  = 'e';
                        magic[7]  = 'n';
                        magic[8]  = 'g';
                        magic[9]  = 'i';
                        magic[10] = 'n';
                        magic[11] = 'e';
                        magic[12] = '_';
                        magic[13] = 'a';
                        magic[14] = 's';
                        magic[15] = 's';
                        magic[16] = 'e';
                        magic[17] = 't';

                        //fh.magic   = magic;
                        memcpy(fh.magic, magic, sizeof(char) * 18);
                        fh.version = RAZIX_ASSET_VERSION;
                        fh.type    = ASSET_MESH;

                        BINMeshFileHeader header{};

                        // Copy Name
                        strcpy(header.name, std::string(import_result.name + submesh.name).c_str());
                        header.name[import_result.name.size()] = '\0';

                        header.index_count           = submesh.index_count;
                        header.vertex_count          = submesh.vertex_count;
                        header.skeletal_vertex_count = static_cast<uint32_t>(submesh.vertex_count);
                        header.material_count        = static_cast<uint32_t>(import_result.materials.size());
                        header.mesh_count            = static_cast<uint32_t>(import_result.submeshes.size());
                        header.blobs_count           = VERTEX_ATTRIBS_COUNT;
                        header.max_extents           = submesh.max_extents;
                        header.min_extents           = submesh.min_extents;
                        header.base_index            = submesh.base_index;
                        header.base_vertex           = submesh.base_vertex;
                        header.material_index        = submesh.material_index;
                        //header.materialName          = submesh.materialName;
                        strcpy(header.materialName, &submesh.materialName[0]);

                        std::cout << "Exporting Mesh... : " << import_result.name + submesh.name << std::endl;

                        size_t offset = 0;

                        // Write Asset File header
                        WRITE_AND_OFFSET(f, (char*) &fh, sizeof(BINFileHeader), offset);

                        // Write mesh header
                        WRITE_AND_OFFSET(f, (char*) &header, sizeof(BINMeshFileHeader), offset);

#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1
                        // Write vertices
                        if (import_result.vertices.size() > 0) {
                            uint32_t start_vtx_idx = submesh.base_vertex;
                            auto&    data          = import_result.vertices;
                            auto     subData       = std::vector<Gfx::RZVertex>(data.begin() + start_vtx_idx, data.begin() + start_vtx_idx + submesh.vertex_count);

                            WRITE_AND_OFFSET(f, (char*) &subData[0], sizeof(Gfx::RZVertex) * submesh.vertex_count, offset);
                        }

                        // Write skeletal vertices
                        if (import_result.skeletal_vertices.size() > 0) {
                            WRITE_AND_OFFSET(f, (char*) &import_result.skeletal_vertices[0], sizeof(Gfx::RZSkeletalVertex) * import_result.skeletal_vertices.size(), offset);
                        }
#endif

                        // Write indices
                        if (import_result.indices.size() > 0) {
                            uint32_t start_idx_idx = submesh.base_index;
                            auto     data          = import_result.indices;
                            auto     subData       = std::vector<uint32_t>(data.begin() + start_idx_idx, data.begin() + start_idx_idx + submesh.index_count);

                            WRITE_AND_OFFSET(f, (char*) &subData[0], sizeof(uint32_t) * submesh.index_count, offset);
                        }

// Write vertex data attrib by attrib
#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V2

                        // write positions
                        BINBlobHeader h;
                        h.stride = sizeof(glm::vec3);
                        strcpy(h.typeName, "POSITION:R32G32B32");
                        u32 writeSize = import_result.vertices.Position.size();
                        h.size        = writeSize > 0 ? submesh.vertex_count * h.stride : 0;
                        WRITE_AND_OFFSET(f, (char*) &h, sizeof(BINBlobHeader), offset);
                        if (h.size > 0)
                            WRITE_AND_OFFSET(f, (char*) &import_result.vertices.Position[submesh.base_vertex], h.size, offset);

                        // Color
                        h        = {};
                        h.stride = sizeof(glm::vec4);
                        strcpy(h.typeName, "COLOR:R32G32B32A32");
                        writeSize = import_result.vertices.Color.size();
                        h.size    = writeSize > 0 ? submesh.vertex_count * h.stride : 0;
                        WRITE_AND_OFFSET(f, (char*) &h, sizeof(BINBlobHeader), offset);
                        if (h.size > 0)
                            WRITE_AND_OFFSET(f, (char*) &import_result.vertices.Color[submesh.base_vertex], h.size, offset);

                        // UV
                        h        = {};
                        h.stride = sizeof(glm::vec2);
                        strcpy(h.typeName, "TEXCOORD:R32G32");
                        writeSize = import_result.vertices.UV.size();
                        h.size    = writeSize > 0 ? submesh.vertex_count * h.stride : 0;
                        WRITE_AND_OFFSET(f, (char*) &h, sizeof(BINBlobHeader), offset);
                        if (h.size > 0)
                            WRITE_AND_OFFSET(f, (char*) &import_result.vertices.UV[submesh.base_vertex], h.size, offset);

                        // Normal
                        h        = {};
                        h.stride = sizeof(glm::vec3);
                        strcpy(h.typeName, "NORMAL:R32G32B32");
                        writeSize = import_result.vertices.Normal.size();
                        h.size    = writeSize > 0 ? submesh.vertex_count * h.stride : 0;
                        WRITE_AND_OFFSET(f, (char*) &h, sizeof(BINBlobHeader), offset);
                        if (h.size > 0)
                            WRITE_AND_OFFSET(f, (char*) &import_result.vertices.Normal[submesh.base_vertex], h.size, offset);

                        // Tangent
                        h        = {};
                        h.stride = sizeof(glm::vec3);
                        strcpy(h.typeName, "TANGENT:R32G32B32");
                        writeSize = import_result.vertices.Tangent.size();
                        h.size    = writeSize > 0 ? submesh.vertex_count * h.stride : 0;
                        WRITE_AND_OFFSET(f, (char*) &h, sizeof(BINBlobHeader), offset);
                        if (h.size > 0)
                            WRITE_AND_OFFSET(f, (char*) &import_result.vertices.Tangent[submesh.base_vertex], h.size, offset);

#endif
                        f.close();

                        // TODO: Export material per submesh
                        if (import_result.materials.size() > 0) {
                            auto materialName = submesh.materialName;
                            auto materialIdx  = submesh.material_index;
                            auto materialData = import_result.materials[materialIdx];

                            std::cout << "Exporting Material ... : " << materialName << std::endl;

                            std::string mat_export_path = materials_path + materialName + ".rzmaterial";

                            // Remove the Export Directory from material textures paths
                            //uint32_t letters_size = static_cast<uint32_t>(materials_path.size());

                            // Needs more improvements and VFS search mechanism
                            //auto path = "//Assets/" + std::string(materialData.m_MaterialTexturePaths.albedo + letters_size);
                            //memcpy(materialData.m_MaterialTexturePaths.albedo, path.c_str(), 250);
                            //path = "//Assets/" + std::string(materialData.m_MaterialTexturePaths.ao + letters_size);
                            //memcpy(materialData.m_MaterialTexturePaths.ao, path.c_str(), 250);
                            //path = "//Assets/" + std::string(materialData.m_MaterialTexturePaths.emissive + letters_size);
                            //memcpy(materialData.m_MaterialTexturePaths.emissive, path.c_str(), 250);
                            //path = "//Assets/" + std::string(materialData.m_MaterialTexturePaths.metallic + letters_size);
                            //memcpy(materialData.m_MaterialTexturePaths.metallic, path.c_str(), 250);
                            //path = "//Assets/" + std::string(materialData.m_MaterialTexturePaths.normal + letters_size);
                            //memcpy(materialData.m_MaterialTexturePaths.normal, path.c_str(), 250);
                            //path = "//Assets/" + std::string(materialData.m_MaterialTexturePaths.roughness + letters_size);
                            //memcpy(materialData.m_MaterialTexturePaths.roughness, path.c_str(), 250);
                            //path = "//Assets/" + std::string(materialData.m_MaterialTexturePaths.specular + letters_size);
                            //memcpy(materialData.m_MaterialTexturePaths.specular, path.c_str(), 250);

                            uint32_t offset = 0;

#ifdef EXPORT_BIN_MATERIAL
                            std::fstream f_mat(mat_export_path, std::ios::out | std::ios::binary);
                            if (f_mat.is_open()) {
                                WRITE_AND_OFFSET(f_mat, (char*) &materialData, sizeof(Razix::Gfx::MaterialData), offset);
                            }
#endif    // EXPORT_BIN_MATERIAL
                            std::ofstream             opAppStream(mat_export_path);
                            cereal::JSONOutputArchive defArchive(opAppStream);
                            defArchive(cereal::make_nvp(materialName, materialData));
                        }

                    } else
                        return false;
                }

                auto                          finish = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> time   = finish - start;

                std::cout << "Successfully Exported mesh in " << time.count() << " seconds" << std::endl;
                return true;
            }
        }    // namespace AssetPacker
    }        // namespace Tool
}    // namespace Razix
