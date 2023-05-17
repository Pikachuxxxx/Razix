#pragma once

namespace Razix {

    namespace Graphics {
        class RZMesh;
        enum MeshPrimitive : int;
    }    // namespace Graphics

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
        Graphics::RZMesh*       Mesh;
        Graphics::MeshPrimitive primitive;

        MeshRendererComponent();
        MeshRendererComponent(const std::string& filePath);
        MeshRendererComponent(Graphics::MeshPrimitive primitive);
        MeshRendererComponent(Graphics::RZMesh* mesh);
        MeshRendererComponent(const MeshRendererComponent&) = default;

        template<class Archive>
        void load(Archive& archive)
        {
            int prim = -1;
            archive(cereal::make_nvp("Primitive", prim));
            primitive = Graphics::MeshPrimitive(prim);
            Mesh      = Graphics::MeshFactory::CreatePrimitive(primitive);
            std::string meshName;
            archive(cereal::make_nvp("MeshName", meshName));
            //Mesh->setName(meshName);
            std::string meshPath;
            archive(cereal::make_nvp("Path", meshPath));
            //Mesh->setPath(meshPath);
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("Primitive", primitive));
            if (Mesh) {
                archive(cereal::make_nvp("MeshName", Mesh->getName()));
                archive(cereal::make_nvp("Path", Mesh->getPath()));
            }
        }
    };

}    // namespace Razix