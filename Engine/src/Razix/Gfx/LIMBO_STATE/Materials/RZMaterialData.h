#pragma once

#include "Razix/Core/RZDataTypes.h"
#include "Razix/Gfx/GfxData.h"
#include "Razix/Core/Utils/RZVendorOverrides.h"

#include <cereal/cereal.hpp>
#include <glm/glm.hpp>

namespace Razix {
    namespace Gfx {
        // Forward declaration
        class RZShader;
        class RZTexture;
        class RZUniformBuffer;
        class RZDescriptorSet;

        /* The type of workflow used by the material for authoring the materials for the different lighting models */
        enum class WorkFlow : u32
        {
            WORKFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED,    // In the order of BGR components! AO = r, Roughness = g, Metal = b
            WORKFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE,
            WORKFLOW_PBR_SPECULAR_GLOSS_COMBINED,
            WORKFLOW_PBR_SPECULAR_GLOSS_SEPARATE,
            WORKFLOW_UNLIT,
            WORKFLOW_LIT_PHONG
        };

        // TODO: Make this work with bool
        /* Material properties (16-byte aligned as per optimal GPU requirements) */
        struct alignas(16) MaterialProperties
        {
            float3 albedoColor  = float3(1.0f, 1.0f, 1.0f);
            bool   _padding0[4] = {};

            float3 normal            = float3(0.0f, 1.0f, 0.0f);
            f32    emissiveIntensity = 1.0f;

            f32 metallicColor  = 1.0f;
            f32 roughnessColor = 0.025f;
            f32 specularColor  = 1.0f;
            f32 opacity        = 1.0f;

            f32    ambientOcclusion = 1.0f;
            float2 uvScale          = {1.0f, 1.0f};
            u32    workflow         = (u32) WorkFlow::WORKFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED;    // Default for GLTF models which are primary source for Razix

            u32 visible = true;
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
            u32 AlbedoMapIdx    = UINT16_MAX;
            u32 NormalMapIdx    = UINT16_MAX;
            u32 MetallicMapIdx  = UINT16_MAX;
            u32 RoughnessMapIdx = UINT16_MAX;
            u32 SpecularIdx     = UINT16_MAX;
            u32 EmissiveMapIdx  = UINT16_MAX;
            u32 AOMapIdx        = UINT16_MAX;
            u32 _padding1;
        };

        /* lighting model textures */
        struct MaterialTextures
        {
            rz_texture_handle albedo;
            rz_texture_handle normal;
            rz_texture_handle metallic;
            rz_texture_handle roughness;
            rz_texture_handle specular;
            rz_texture_handle emissive;
            rz_texture_handle ao;

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

                auto safe_strncpy = [](char* dest, const std::string& src, size_t maxLength) {
                    std::strncpy(dest, src.c_str(), maxLength);
                    dest[maxLength] = '\0';    // Ensure null termination
                };

                // Copy safely using the helper function
                safe_strncpy(m_Name, name, sizeof(m_Name) - 1);
                safe_strncpy(m_MaterialTexturePaths.albedo, albedo, sizeof(m_MaterialTexturePaths.albedo) - 1);
                safe_strncpy(m_MaterialTexturePaths.ao, ao, sizeof(m_MaterialTexturePaths.ao) - 1);
                safe_strncpy(m_MaterialTexturePaths.emissive, emissive, sizeof(m_MaterialTexturePaths.emissive) - 1);
                safe_strncpy(m_MaterialTexturePaths.metallic, metallic, sizeof(m_MaterialTexturePaths.metallic) - 1);
                safe_strncpy(m_MaterialTexturePaths.normal, normal, sizeof(m_MaterialTexturePaths.normal) - 1);
                safe_strncpy(m_MaterialTexturePaths.roughness, roughness, sizeof(m_MaterialTexturePaths.roughness) - 1);
                safe_strncpy(m_MaterialTexturePaths.specular, specular, sizeof(m_MaterialTexturePaths.specular) - 1);
            }
        };
    }    // namespace Gfx
}    // namespace Razix
