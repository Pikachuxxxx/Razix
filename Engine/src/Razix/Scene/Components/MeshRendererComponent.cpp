// clang-format off
#include "rzxpch.h"
// clang-format on
#include "MeshRendererComponent.h"

namespace Razix {

    MeshRendererComponent::MeshRendererComponent()
        : Mesh(nullptr)
    {
    }
    MeshRendererComponent::MeshRendererComponent(Gfx::MeshPrimitive primitive)
        : Mesh(Gfx::MeshFactory::CreatePrimitive(primitive)), primitive(primitive)
    {
    }
    MeshRendererComponent::MeshRendererComponent(Gfx::RZMesh* mesh)
        : Mesh(mesh)
    {
    }

    MeshRendererComponent::MeshRendererComponent(const std::string& filePath)
    {
        Mesh = Razix::Gfx::loadMesh(filePath);
    }
}    // namespace Razix