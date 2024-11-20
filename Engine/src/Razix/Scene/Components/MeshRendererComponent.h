#pragma once

#include "Razix/Gfx/Loaders/RZMeshLoader.h"
#include "Razix/Gfx/Materials/RZMaterial.h"
#include "Razix/Gfx/RZGraphicsCompileConfig.h"
#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZMeshFactory.h"

#include <cereal/cereal.hpp>

namespace Razix {

    namespace Gfx {
        class RZMesh;
        enum MeshPrimitive : int;
        class RZMaterial;
    }    // namespace Graphics

    /**
     * Mesh renderer component references a mesh that will taken by the render to render a mesh on the 3D scene
     * It holds the reference to a 3D model or a primitive mesh to be rendered, so if a model is instantiated as
     * a Entity in the scene, each of it's children(which are also entities) will have a mesh renderer component
     * will be instantiated as entities in the scene with a Hierarchy, Transform,Tag, Active and default components
     * attached to them, these all meshes are taken at once by the Renderer and rendered to the scene, any additional
     * information required to rendered can be inferred as needed
     */
    struct RAZIX_API MeshRendererComponent
    {
        Gfx::RZMesh*       Mesh;
        Gfx::MeshPrimitive primitive;

        glm::mat4 PreviousWorldTransform;    // Run-time variable for storing the previous frame world matrix

        bool enableBoundingBoxes = false;
        bool receiveShadows      = true;

        MeshRendererComponent();
        MeshRendererComponent(const std::string& filePath);
        MeshRendererComponent(Gfx::MeshPrimitive primitive);
        MeshRendererComponent(Gfx::RZMesh* mesh);
        MeshRendererComponent(const MeshRendererComponent&) = default;

        template<class Archive>
        void load(Archive& archive)
        {
            int prim = -1;
            archive(cereal::make_nvp("Primitive", prim));
            primitive = Gfx::MeshPrimitive(prim);

            if (prim >= 0)
                Mesh = Gfx::MeshFactory::CreatePrimitive(primitive);
            std::string meshName;
            archive(cereal::make_nvp("MeshName", meshName));
            std::string meshPath;
            archive(cereal::make_nvp("MeshPath", meshPath));

            if (!Mesh || !meshPath.empty())
                Mesh = Razix::Gfx::loadMesh(meshPath);

            if (Mesh) {
                Mesh->setName(meshName);
                Mesh->setPath(meshPath);
            }

            // Load/Create a new Material (override the save location)
            std::string materialPath;
            archive(cereal::make_nvp("MaterialPath", materialPath));
#if !DISABLE_MATERIALS_LOADING
            if (!materialPath.empty()) {
                // Since we have the path to a material file load it, deserialize it and create the material
                Mesh->getMaterial()->loadFromFile(materialPath);
            }
#endif
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("Primitive", primitive));
            if (Mesh) {
                archive(cereal::make_nvp("MeshName", Mesh->getName()));
                archive(cereal::make_nvp("MeshPath", Mesh->getPath()));

                auto matPath = "//Assets/Materials/" + Mesh->getMaterial()->getName() + ".rzmaterial";
                archive(cereal::make_nvp("MaterialPath", matPath));
                Mesh->getMaterial()->saveToFile();
            } else {
                std::string Dummy = "Dummy";
                archive(cereal::make_nvp("MeshName", Dummy));
                archive(cereal::make_nvp("MeshPath", Dummy));
                archive(cereal::make_nvp("MaterialPath", Dummy));
            }
        }
    };
}    // namespace Razix