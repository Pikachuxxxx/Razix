#pragma once

namespace Razix {

    namespace Graphics {
        class RZMesh;
        enum class MeshPrimitive: int;
    }

    /**
     * Mesh renderer component references a mesh that will taken by the render to render a mesh on the 3D scene
     * It holds the reference to a 3D model or a primitive mesh to be rendered, so if a model is instantiated as
     * a Entity in the scene, each of it's children(which are also entities) will have a mesh renderer component
     *  will be instantiated as entities in the scene with a Hierarchy, Transform,Tag, Active and default components
     * attached to them, these all meshes are taken at once by the Renderer and rendered to the scene, any additional
     * information required to rendered can be inferred as needed
     */
    struct RAZIX_API MeshRendererComponent
    {
        Graphics::RZMesh* Mesh;

        MeshRendererComponent();
        MeshRendererComponent(Graphics::MeshPrimitive primitive);
        MeshRendererComponent(Graphics::RZMesh* mesh);
        MeshRendererComponent(const MeshRendererComponent&) = default;

        template<class Archive>
        void load(Archive& archive)
        {
            if (Mesh) {
                std::string meshName;
                archive(cereal::make_nvp("MeshName", meshName));
                Mesh->setName(meshName);
            }
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            if (Mesh)
                archive(cereal::make_nvp("MeshName", Mesh->getName()));
        }
    };

}    // namespace Razix