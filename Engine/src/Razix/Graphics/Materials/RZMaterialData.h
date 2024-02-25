#pragma once

#include "Razix/Core/RZDataTypes.h"
#include "Razix/Utilities/RZVendorOverrides.h"

#include "Razix/Graphics/RHI/API/RZAPIHandles.h"

#include <cereal/cereal.hpp>
#include <glm/glm.hpp>

namespace Razix {
    namespace Graphics {
        // Forward declaration
        class RZShader;
        class RZTexture;
        class RZUniformBuffer;
        class RZDescriptorSet;

        /* The type of workflow used by the material for authoring the materials for the different lighting models */
        enum class WorkFlow : u32
        {
            WORLFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED,    // In the order of BGR components! AO = r, Roughness = g, Metal = b
            WORLFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE,
            WORLFLOW_PBR_SPECULAR_GLOSS_COMBINED,
            WORLFLOW_PBR_SPECULAR_GLOSS_SEPARATE,
            WORKFLOW_UNLIT,
            WORLFLOW_LIT_PHONG
        };

        // TODO: Make this work with bool
        /* Material properties (16-byte aligned as per optimal GPU requirements) */
        struct alignas(16) MaterialProperties
        {
            glm::vec3 albedoColor = glm::vec3(1.0f, 1.0f, 1.0f);
            bool      _padding[4] = {};
            glm::vec3 normal      = glm::vec3(0.0f, 1.0f, 0.0f);
            //bool      _padding_[4]        = {1, 0, 1, 0};
            f32       emissiveIntensity = 1.0f;
            f32       metallicColor     = 1.0f;
            f32       roughnessColor    = 0.025f;
            f32       specularColor     = 1.0f;
            f32       opacity           = 1.0f;
            f32       ambientOcclusion  = 1.0f;
            bool      _padding2[4]      = {};
            glm::vec2 uvScale           = {1.0f, 1.0f};
            u32       visible           = true;
            u32       workflow          = (u32) WorkFlow::WORLFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED;    // Default for GLTF models which are primary source for Razix
            // TODO: Use these as bindless array indices
            u32 isUsingAlbedoMap    = false;
            u32 isUsingNormalMap    = false;
            u32 isUsingMetallicMap  = false;
            u32 isUsingRoughnessMap = false;
            u32 isUsingSpecular     = false;
            u32 isUsingEmissiveMap  = false;
            u32 isUsingAOMap        = false;
            //------------------------------
            // RUNTIME STUFF
            u32 AlbedoMapIdx    = u16_max;
            u32 NormalMapIdx    = u16_max;
            u32 MetallicMapIdx  = u16_max;
            u32 RoughnessMapIdx = u16_max;
            u32 SpecularIdx     = u16_max;
            u32 EmissiveMapIdx  = u16_max;
            u32 AOMapIdx        = u16_max;
        };

        /* lighting model textures */
        struct MaterialTextures
        {
            RZTextureHandle albedo;
            RZTextureHandle normal;
            RZTextureHandle metallic;
            RZTextureHandle roughness;
            RZTextureHandle specular;
            RZTextureHandle emissive;
            RZTextureHandle ao;

            void Destroy();
        };

        struct MaterialTexturePaths
        {
            char albedo[250] = {};
            char normal[250] = {};
            union
            {
                char metallic[250] = {};
                char metallicRoughnessAO[250];
            };
            char roughness[250] = {};
            char specular[250]  = {};
            char emissive[250]  = {};
            char ao[250]        = {};
        };

#if 1
        struct MaterialData
        {
            char                 m_Name[250]            = "MT_PBR.Lit.Default";
            MaterialProperties   m_MaterialProperties   = {};
            MaterialTexturePaths m_MaterialTexturePaths = {};
            MaterialTextures     m_MaterialTextures;

            // NOTE: This probably won't be used, we will use binary files instead
            template<typename Archive>
            void save(Archive& archive) const
            {
    #if 1
                archive(cereal::make_nvp("Name", std::string(m_Name)),
                    cereal::make_nvp("Albedo", m_MaterialTexturePaths.albedo ? std::string(m_MaterialTexturePaths.albedo) : ""),
                    cereal::make_nvp("Normal", m_MaterialTexturePaths.normal ? std::string(m_MaterialTexturePaths.normal) : ""),
                    cereal::make_nvp("Metallic", m_MaterialTexturePaths.metallic ? std::string(m_MaterialTexturePaths.metallic) : ""),
                    cereal::make_nvp("Roughness", m_MaterialTexturePaths.roughness ? std::string(m_MaterialTexturePaths.roughness) : ""),
                    cereal::make_nvp("Ao", m_MaterialTexturePaths.ao ? std::string(m_MaterialTexturePaths.ao) : ""),
                    cereal::make_nvp("Emissive", m_MaterialTexturePaths.emissive ? std::string(m_MaterialTexturePaths.emissive) : ""),
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
                std::string albedo;
                std::string normal;
                std::string metallic;
                std::string roughness;
                std::string specular;
                std::string emissive;
                std::string ao;
                std::string name;

    #if 1
                archive(cereal::make_nvp("Name", name),
                    cereal::make_nvp("Albedo", albedo),
                    cereal::make_nvp("Normal", normal),
                    cereal::make_nvp("Metallic", metallic),
                    cereal::make_nvp("Roughness", roughness),
                    cereal::make_nvp("Ao", ao),
                    cereal::make_nvp("Emissive", emissive),
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

                memcpy(m_Name, name.c_str(), 250);
                memcpy(m_MaterialTexturePaths.albedo, albedo.c_str(), 250);
                memcpy(m_MaterialTexturePaths.ao, ao.c_str(), 250);
                memcpy(m_MaterialTexturePaths.emissive, emissive.c_str(), 250);
                memcpy(m_MaterialTexturePaths.metallic, metallic.c_str(), 250);
                memcpy(m_MaterialTexturePaths.normal, normal.c_str(), 250);
                memcpy(m_MaterialTexturePaths.roughness, roughness.c_str(), 250);
                memcpy(m_MaterialTexturePaths.specular, specular.c_str(), 250);
    #endif
            }
        };
#endif
    }    // namespace Graphics
}    // namespace Razix