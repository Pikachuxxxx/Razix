// clang-format off
#include "rzxpch.h"
// clang-format on
#include "MeshRendererComponent.h"

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
}    // namespace Razix