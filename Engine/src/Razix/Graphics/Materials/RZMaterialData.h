#pragma once

#include "Razix/Core/RZDataTypes.h"

#include <glm/glm.hpp>

namespace Razix {
    namespace Graphics {
        // Forward declaration
        class RZShader;
        class RZTexture2D;
        class RZUniformBuffer;
        class RZDescriptorSet;

        /* The type of workflow used by the material for authoring the materials for the PBR lighting models */
        enum class WorkFlow
        {
            PBR_WORKFLOW_SEPARATE_TEXTURES,
            PBR_WORKFLOW_METALLIC_ROUGHTNESS,
            PBR_WORKFLOW_SPECULAR_ROUGHNESS
        };

        // TODO: Make this work with bool
        /* Material properties (16-byte aligned as per optimal GPU requirements) */
        struct MaterialProperties
        {
            glm::vec3 albedoColor = glm::vec3(1.0f, 1.0f, 1.0f);
            bool      _padding[4] = {};
            glm::vec3 normal      = glm::vec3(0.0f, 1.0f, 0.0f);
            //bool      _padding_[4]        = {1, 0, 1, 0};
            f32 emissiveIntensity   = 1.0f;
            f32 metallicColor       = 1.0f;
            f32 roughnessColor      = 0.025f;
            f32 specularColor       = 1.0f;
            f32 opacity             = 1.0f;
            f32 ambientOcclusion    = 1.0f;
            u32 visible             = true;
            u32 isUsingAlbedoMap    = false;
            u32 isUsingNormalMap    = false;
            u32 isUsingMetallicMap  = false;
            u32 isUsingRoughnessMap = false;
            u32 isUsingSpecular     = false;
            u32 isUsingEmissiveMap  = false;
            u32 isUsingAOMap        = false;
        };

        /* lighting model textures */
        struct MaterialTextures
        {
            RZTexture2D* albedo    = nullptr;
            RZTexture2D* normal    = nullptr;
            RZTexture2D* metallic  = nullptr;
            RZTexture2D* roughness = nullptr;
            RZTexture2D* specular  = nullptr;
            RZTexture2D* emissive  = nullptr;
            RZTexture2D* ao        = nullptr;

            void Destroy();
        };

        struct MaterialTexturePaths
        {
            char albedo[250];
            char normal[250];
            char metallic[250];
            char roughness[250];
            char specular[250];
            char emissive[250];
            char ao[250];
        };

#if 1
        struct MaterialData
        {
            char                 m_Name[250];
            MaterialProperties   m_MaterialProperties;
            MaterialTexturePaths m_MaterialTextures;

            // NOTE: This probably won't be used, we will use binary files instead
            template<typename Archive>
            void save(Archive& archive) const
            {
    #if 1
                archive(cereal::make_nvp("Name", m_Name),
                    cereal::make_nvp("Albedo", m_MaterialTextures.albedo ? m_MaterialTextures.albedo : ""),
                    cereal::make_nvp("Normal", m_MaterialTextures.normal ? m_MaterialTextures.normal : ""),
                    cereal::make_nvp("Metallic", m_MaterialTextures.metallic ? m_MaterialTextures.metallic : ""),
                    cereal::make_nvp("Roughness", m_MaterialTextures.roughness ? m_MaterialTextures.roughness : ""),
                    cereal::make_nvp("Ao", m_MaterialTextures.ao ? m_MaterialTextures.ao : ""),
                    cereal::make_nvp("Emissive", m_MaterialTextures.emissive ? m_MaterialTextures.emissive : ""),
                    cereal::make_nvp("albedoColor", m_MaterialProperties.albedoColor),
                    cereal::make_nvp("roughnessColor", m_MaterialProperties.roughnessColor),
                    cereal::make_nvp("metallicColor", m_MaterialProperties.metallicColor),
                    cereal::make_nvp("emissiveColor", m_MaterialProperties.emissiveIntensity),
                    cereal::make_nvp("isUsingAlbedoMap", m_MaterialProperties.isUsingAlbedoMap),
                    cereal::make_nvp("isUsingMetallicMap", m_MaterialProperties.isUsingMetallicMap),
                    cereal::make_nvp("isUsingRoughnessMap", m_MaterialProperties.isUsingRoughnessMap),
                    cereal::make_nvp("isUsingNormalMap", m_MaterialProperties.isUsingNormalMap),
                    cereal::make_nvp("isUsingAOMap", m_MaterialProperties.isUsingAOMap),
                    cereal::make_nvp("isUsingEmissiveMap", m_MaterialProperties.isUsingEmissiveMap));
    #endif
            }

            template<typename Archive>
            void load(Archive& archive)
            {
    #if 1
                archive(cereal::make_nvp("Name", m_Name),
                    cereal::make_nvp("Albedo", m_MaterialTextures.albedo),
                    cereal::make_nvp("Normal", m_MaterialTextures.normal),
                    cereal::make_nvp("Metallic", m_MaterialTextures.metallic),
                    cereal::make_nvp("Roughness", m_MaterialTextures.roughness),
                    cereal::make_nvp("Ao", m_MaterialTextures.ao),
                    cereal::make_nvp("Emissive", m_MaterialTextures.emissive),
                    cereal::make_nvp("albedoColor", m_MaterialProperties.albedoColor),
                    cereal::make_nvp("roughnessColor", m_MaterialProperties.roughnessColor),
                    cereal::make_nvp("metallicColor", m_MaterialProperties.metallicColor),
                    cereal::make_nvp("emissiveColor", m_MaterialProperties.emissiveIntensity),
                    cereal::make_nvp("isUsingAlbedoMap", m_MaterialProperties.isUsingAlbedoMap),
                    cereal::make_nvp("isUsingMetallicMap", m_MaterialProperties.isUsingMetallicMap),
                    cereal::make_nvp("isUsingRoughnessMap", m_MaterialProperties.isUsingRoughnessMap),
                    cereal::make_nvp("isUsingNormalMap", m_MaterialProperties.isUsingNormalMap),
                    cereal::make_nvp("isUsingAOMap", m_MaterialProperties.isUsingAOMap),
                    cereal::make_nvp("isUsingEmissiveMap", m_MaterialProperties.isUsingEmissiveMap));
    #endif
            }
        };
#endif

    }    // namespace Graphics
}    // namespace Razix