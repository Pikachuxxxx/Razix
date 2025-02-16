// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZMeshLoader.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/AssetSystem/RZAssetFileSpec.h"

#include "Razix/Gfx/RZGraphicsCompileConfig.h"
#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Materials/RZMaterial.h"

#include <cereal/archives/json.hpp>

#define READ_AND_OFFSET(stream, dest, size, offset) \
    stream.read((char*) dest, size);                \
    offset += size;                                 \
    stream.seekg(offset);

#define SKIP_AND_OFFSET(stream, dest, size, offset) \
    offset += size;                                 \
    stream.seekg(offset);

namespace Razix {
    namespace Gfx {

        static void VerifyMeshBinFileHeader(Razix::AssetSystem::BINFileHeader header)
        {
            // Verify Magic number then the version and type
            char magicNumberBuf[MAGIC_NUM_SZ] = {};
            memcpy(magicNumberBuf, header.magic, MAGIC_NUM_SZ);
            RAZIX_ASSERT(strcmp(magicNumberBuf, "razix_engine_asset"), "[Mesh Loader] Unknown magic number, this is not a valid razix asset file!");
            RAZIX_ASSERT(header.version == RAZIX_ASSET_VERSION, "[Mesh Loader], wrong asset version, only V2 assets are supports, please recompile the assets with latest.");
            RAZIX_ASSERT(header.type == (uint8_t) Razix::AssetSystem::ASSET_MESH, "[Mesh Loader] Asset is not a mesh, it will fail to load!");
        }

        RZMesh* loadMesh(const std::string& filePath)
        {
            RZMesh* mesh = nullptr;

            // Check if it's Virtual path and resolve it
            std::string physicalPath;
            if (!RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, physicalPath))
                return nullptr;

            std::fstream f(physicalPath, std::ios::in | std::ios::binary);

            if (!f.is_open())
                return nullptr;

            Razix::AssetSystem::BINFileHeader     file_header{};
            Razix::AssetSystem::BINMeshFileHeader mesh_header{};

            size_t offset = 0;

            READ_AND_OFFSET(f, &file_header, sizeof(Razix::AssetSystem::BINFileHeader), offset);

            VerifyMeshBinFileHeader(file_header);

            READ_AND_OFFSET(f, &mesh_header, sizeof(Razix::AssetSystem::BINMeshFileHeader), offset);

            // This is dummy Name until we load the submesh and get it's true name, this is use the by Root empty Entity that will hold meshes unless it's a single one

            Gfx::RZVertex         vertices;
            std::vector<uint32_t> indices(mesh_header.index_count);

            if (mesh_header.index_count > 0) {
                READ_AND_OFFSET(f, (char*) &indices[0], sizeof(uint32_t) * mesh_header.index_count, offset);
            }

            if (mesh_header.vertex_count > 0) {
                // Load vertices
                Razix::AssetSystem::BINBlobHeader h;
                h = {};
                READ_AND_OFFSET(f, (char*) &h, sizeof(Razix::AssetSystem::BINBlobHeader), offset);
                if (h.size > 0) {
                    vertices.Position.resize(mesh_header.vertex_count);
                    READ_AND_OFFSET(f, (char*) &vertices.Position[0], h.size, offset);
                }

                // Color
                h = {};
                READ_AND_OFFSET(f, (char*) &h, sizeof(Razix::AssetSystem::BINBlobHeader), offset);
                if (h.size > 0) {
                    vertices.Color.resize(mesh_header.vertex_count);
                    READ_AND_OFFSET(f, (char*) &vertices.Color[0], h.size, offset);
                }

                // UV
                h = {};
                READ_AND_OFFSET(f, (char*) &h, sizeof(Razix::AssetSystem::BINBlobHeader), offset);
                if (h.size > 0) {
                    vertices.UV.resize(mesh_header.vertex_count);
                    READ_AND_OFFSET(f, (char*) &vertices.UV[0], h.size, offset);
                }

                // Normal
                h = {};
                READ_AND_OFFSET(f, (char*) &h, sizeof(Razix::AssetSystem::BINBlobHeader), offset);
                if (h.size > 0) {
                    vertices.Normal.resize(mesh_header.vertex_count);
                    READ_AND_OFFSET(f, (char*) &vertices.Normal[0], h.size, offset);
                }

                // Tangent
                h = {};
                READ_AND_OFFSET(f, (char*) &h, sizeof(Razix::AssetSystem::BINBlobHeader), offset);
                if (h.size > 0) {
                    vertices.Tangent.resize(mesh_header.vertex_count);
                    READ_AND_OFFSET(f, (char*) &vertices.Tangent[0], h.size, offset);
                }
            }

            if (mesh_header.skeletal_vertex_count > 0) {
                SKIP_AND_OFFSET(f, nullptr, sizeof(Gfx::RZSkeletalVertex) * mesh_header.skeletal_vertex_count, offset);
            }

            // Load Materials
            RAZIX_CORE_TRACE("Loading material : {0}", mesh_header.materialName);

            // Create the Mesh
            mesh = new RZMesh(vertices, indices);

            mesh->setName(mesh_header.name);
            mesh->setMaxExtents(mesh_header.max_extents);
            mesh->setMinExtents(mesh_header.min_extents);
            mesh->setPath(filePath);
            //mesh->setBaseIndex(mesh_header.base_index);
            //mesh->setBaseVertex(mesh_header.base_vertex);

            auto material = loadMaterial(mesh_header.materialName, std::string(mesh_header.name));

            mesh->setMaterial(material);

            indices.clear();

            return mesh;
        }

        RZMaterial* loadMaterial(const std::string& materialName, const std::string& folderName)
        {
#if DISABLE_MATERIALS_LOADING
            return RZMaterial::GetDefaultMaterial();
#else

            if (materialName == "DefaultMaterial")
                return GetDefaultMaterial();

            auto        shader   = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);
            RZMaterial* material = new RZMaterial(shader);
            material->setName(materialName);

            // Using the Name of the Material search the //Assets/Materials + MeshName_MaterialName.rzmaterial file and load it and set the material Data
            std::string              matPath = "//Assets/Materials/" + folderName + "/" + std::string(materialName) + ".rzmaterial";
            Razix::Gfx::MaterialData matData{};
            std::string              matPhysicalPath;
            if (RZVirtualFileSystem::Get().resolvePhysicalPath(matPath, matPhysicalPath)) {
                if (!matPhysicalPath.empty()) {
                    std::ifstream AppStream;
                    AppStream.open(matPhysicalPath, std::ifstream::in);
                    cereal::JSONInputArchive inputArchive(AppStream);

                    inputArchive(cereal::make_nvp(materialName, matData));
                }
            }

            material->setProperties(matData.m_MaterialProperties);
            material->loadMaterialTexturesFromFiles(matData.m_MaterialTexturePaths);
            material->createDescriptorSet();

            return material;
#endif
        }
    }    // namespace Gfx
}    // namespace Razix
