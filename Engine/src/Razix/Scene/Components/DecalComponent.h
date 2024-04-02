#pragma once

#include <cereal/cereal.hpp>

namespace Razix {

    struct DecalInstance
    {
        std::string name;
        std::string diffuseMapPath;
        std::string normalMapPath;
        std::string specularMapPath;
        std::string emissiveMapPath;
        glm::vec4   color;
        f32         opacity;
        u32         normalThreshold;
        u32         layerPriority;

        // Runtime instance of the textures
        Graphics::RZTextureHandle diffuseMap;
        Graphics::RZTextureHandle normalMap;
        Graphics::RZTextureHandle specularMap;
        Graphics::RZTextureHandle emissiveMap;

        template<class Archive>
        void load(Archive& archive)
        {
            archive(cereal::make_nvp("name", name));
            archive(cereal::make_nvp("diffuseMap", diffuseMapPath));
            archive(cereal::make_nvp("normalMap", normalMapPath));
            archive(cereal::make_nvp("specularMap", specularMapPath));
            archive(cereal::make_nvp("emissiveMap", emissiveMapPath));
            archive(cereal::make_nvp("color", color));
            archive(cereal::make_nvp("opacity", opacity));
            archive(cereal::make_nvp("normalThreshold", normalThreshold));
            archive(cereal::make_nvp("layerPriority", layerPriority));

            // Create the textures
            // FIXME Only diffuse and normal for now, they are mandatory

        }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("name", name));
            archive(cereal::make_nvp("diffuseMap", diffuseMapPath));
            archive(cereal::make_nvp("normalMap", normalMapPath));
            archive(cereal::make_nvp("specularMap", specularMapPath));
            archive(cereal::make_nvp("emissiveMap", emissiveMapPath));
            archive(cereal::make_nvp("color", color));
            archive(cereal::make_nvp("opacity", opacity));
            archive(cereal::make_nvp("normalThreshold", normalThreshold));
            archive(cereal::make_nvp("layerPriority", layerPriority));
        }
    };

    /**
     * Decals component to render screen space deferred decals in the scene
     */
    struct RAZIX_API DecalComponent
    {
        std::string   decalFilePath;
        DecalInstance instance;

        DecalComponent()                      = default;
        DecalComponent(const DecalComponent&) = default;
        DecalComponent(const std::string& decalFilePath)
            : decalFilePath(decalFilePath)
        {
        }

        template<class Archive>
        void load(Archive& archive)
        {
            archive(cereal::make_nvp("DecalFilePath", decalFilePath));
            // Load the instance file and create a runtime instance
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("DecalFilePath", decalFilePath));
        }
    };
}    // namespace Razix