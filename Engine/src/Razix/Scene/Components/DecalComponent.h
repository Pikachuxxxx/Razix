#pragma once

#include <cereal/cereal.hpp>

#include "Razix/Graphics/Decal/RZDecal.h"

namespace Razix {

    /**
     * Decals component to render screen space deferred decals in the scene
     */
    struct RAZIX_API DecalComponent
    {
        std::string       decalFilePath;
        Graphics::RZDecal instance;

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