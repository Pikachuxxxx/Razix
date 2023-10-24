// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZMeshLoader.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include <cereal/archives/json.hpp>

#define READ_AND_OFFSET(stream, dest, size, offset) \
    stream.read((char*) dest, size);                \
    offset += size;                                 \
    stream.seekg(offset);

#define SKIP_AND_OFFSET(stream, dest, size, offset) \
    offset += size;                                 \
    stream.seekg(offset);

namespace Razix {
    namespace Graphics {

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

            READ_AND_OFFSET(f, &mesh_header, sizeof(Razix::AssetSystem::BINMeshFileHeader), offset);

            // This is dummy Name until we load the submesh and get it's true name, this is use the by Root empty Entity that will hold meshes unless it's a single one

            std::vector<Graphics::RZVertex> vertices(mesh_header.vertex_count);
            std::vector<uint32_t>           indices(mesh_header.index_count);

            if (mesh_header.vertex_count > 0) {
                READ_AND_OFFSET(f, (char*) &vertices[0], sizeof(Graphics::RZVertex) * mesh_header.vertex_count, offset);
            }

            if (mesh_header.skeletal_vertex_count > 0) {
                SKIP_AND_OFFSET(f, nullptr, sizeof(Graphics::RZSkeletalVertex) * mesh_header.skeletal_vertex_count, offset);
            }

            if (mesh_header.index_count > 0) {
                READ_AND_OFFSET(f, (char*) &indices[0], sizeof(uint32_t) * mesh_header.index_count, offset);
            }

            // TODO: Load Materials
            //RAZIX_CORE_TRACE("Loading material : {0}", mesh_header.materialName);

            // Create the Mesh
            mesh = new RZMesh(indices, vertices);

            mesh->setName(mesh_header.name);
            mesh->setMaxExtents(mesh_header.max_extents);
            mesh->setMinExtents(mesh_header.min_extents);
            mesh->setPath(filePath);
            //mesh->setBaseIndex(mesh_header.base_index);
            //mesh->setBaseVertex(mesh_header.base_vertex);

            auto material = loadMaterial(mesh_header.materialName, std::string(mesh_header.name));

            mesh->setMaterial(material);

            vertices.clear();
            indices.clear();

            return mesh;
        }

        RZMaterial* loadMaterial(const std::string& materialName, const std::string& folderName)
        {
            if (materialName == "DefaultMaterial")
                return GetDefaultMaterial();

            auto        shader   = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);
            RZMaterial* material = new RZMaterial(shader);
            material->setName(materialName);

            // Using the Name of the Material search the //Assets/Materials + MeshName_MaterialName.rzmaterial file and load it and set the material Data
            std::string                   matPath = "//Assets/Materials/" + folderName + "/" + std::string(materialName) + ".rzmaterial";
            Razix::Graphics::MaterialData matData{};
            std::string                   matPhysicalPath;
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
        }
    }    // namespace Graphics
}    // namespace Razix