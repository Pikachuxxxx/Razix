// clang-format off
#include "rzxpch.h"
// clang-format on
#include "MeshRendererComponent.h"

#include "Razix/Graphics/Loaders/RZMeshLoader.h"
#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include <cereal/cereal.hpp>

namespace Razix {

    MeshRendererComponent::MeshRendererComponent()
        : Mesh(nullptr)
    {
    }
    MeshRendererComponent::MeshRendererComponent(Graphics::MeshPrimitive primitive)
        : Mesh(Graphics::MeshFactory::CreatePrimitive(primitive)), primitive(primitive)
    {
    }
    MeshRendererComponent::MeshRendererComponent(Graphics::RZMesh* mesh)
        : Mesh(mesh)
    {
    }

    MeshRendererComponent::MeshRendererComponent(const std::string& filePath)
    {
        Mesh = Razix::Graphics::loadMesh(filePath);
    }

    template<class Archive>
    void MeshRendererComponent::load(Archive& archive)
    {
        int prim = -1;
        archive(cereal::make_nvp("Primitive", prim));
        primitive = Graphics::MeshPrimitive(prim);

        if (prim >= 0)
            Mesh = Graphics::MeshFactory::CreatePrimitive(primitive);
        std::string meshName;
        archive(cereal::make_nvp("MeshName", meshName));
        std::string meshPath;
        archive(cereal::make_nvp("MeshPath", meshPath));

        if (!Mesh || !meshPath.empty())
            Mesh = Razix::Graphics::loadMesh(meshPath);

        if (Mesh) {
            Mesh->setName(meshName);
            Mesh->setPath(meshPath);
        }
    }

    template<class Archive>
    void MeshRendererComponent::save(Archive& archive) const
    {
        archive(cereal::make_nvp("Primitive", primitive));
        if (Mesh) {
            archive(cereal::make_nvp("MeshName", Mesh->getName()));
            archive(cereal::make_nvp("MeshPath", Mesh->getPath()));
        }
    }

}    // namespace Razix