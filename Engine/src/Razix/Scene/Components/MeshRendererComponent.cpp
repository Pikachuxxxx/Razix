// clang-format off
#include "rzxpch.h"
// clang-format on
#include "MeshRendererComponent.h"

#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Graphics/RZModel.h"

namespace Razix {

    MeshRendererComponent::MeshRendererComponent()
        : Mesh(nullptr) {}
    MeshRendererComponent::MeshRendererComponent(Graphics::MeshPrimitive primitive)
        : Mesh(Graphics::MeshFactory::CreatePrimitive(primitive)), primitive(primitive) {}
    MeshRendererComponent::MeshRendererComponent(Graphics::RZMesh* mesh)
        : Mesh(mesh) {}

}    // namespace Razix