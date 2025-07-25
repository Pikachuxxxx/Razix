#pragma once

#include "Razix/Gfx/RZGfxUtil.h"

namespace Razix {
    namespace Gfx {

        struct RZDecal
        {
            std::string name;
            std::string diffuseMapPath;
            std::string normalMapPath;
            std::string specularMapPath;
            std::string emissiveMapPath;
            float4      color;
            f32         opacity;
            u32         normalThreshold;
            u32         layerPriority;

            // Runtime instance of the textures
            rz_gfx_texture_handle diffuseMap;
            rz_gfx_texture_handle normalMap;
            rz_gfx_texture_handle specularMap;
            rz_gfx_texture_handle emissiveMap;

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
    }    // namespace Gfx
}    // namespace Razix