#pragma once

#include "Razix/Core/RZSmartPointers.h"

namespace Razix {
    namespace Graphics {

        /**
         * All the sets at index 0 is for View Projection UBO (no samplers)
         * Set index 1 is for Lighting Buffers only (no textures)
         * Set index 2 is for Material buffer data + Textures (How do we fit sampler and fill the data it's a bit of messy for now)
         * other sets will be allocated for GI, Decals etc up to 32/16
         */
#define SYSTEM_SET_INDEX_VIEW_PROJ_DATA 0
#define SYSTEM_SET_INDEX_LIGHTING_DATA  1
#define SYSTEM_SET_INDEX_MATERIAL_DATA  2

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

        struct RZMaterialProperties
        {
            alignas(16) glm::vec4 albedoColor       = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            alignas(16) glm::vec4 roughnessColor    = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            alignas(16) glm::vec4 metallicColor     = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            alignas(16) glm::vec4 emissiveColor     = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            alignas(16) bool isUsingAlbedoMap       = true;
            alignas(16) bool isUsingNormalMap       = false;
            alignas(16) bool isUsingMetallicMap     = false;
            alignas(16) bool isUsingRoughnessMap    = false;
            alignas(16) bool isUsingAOMap           = false;
            alignas(16) bool isUsingEmissiveMap     = false;
            alignas(16) WorkFlow workflow           = WorkFlow::PBR_WORKFLOW_SEPARATE_TEXTURES;
        };

        struct PBRMataterialTextures
        {
            RZTexture2D* albedo;
            RZTexture2D* normal;
            RZTexture2D* metallic;
            RZTexture2D* roughness;
            RZTexture2D* ao;
            RZTexture2D* emissive;
        };

        /* Not an interface yet, this is a hard coded PBR material as of this iteration of the engine */
        class RAZIX_API RZMaterial
        {
        public:
            /* Crates the material with the given shader and it's properties */
            RZMaterial(RZShader* shader, RZMaterialProperties matProps = RZMaterialProperties(), PBRMataterialTextures textures = PBRMataterialTextures());
            virtual ~RZMaterial() {}

            static void InitDefaultTexture();
            static void ReleaseDefaultTexture();

            /* Overrides the default material properties and textures by loading the material file and de-serializing it */
            void loadMaterial(const std::string& name, const std::string& path);
            void loadShader(const std::string& path);
            void createDescriptorSet();
            void setTextures(PBRMataterialTextures& textures);

            void Bind();

            RAZIX_INLINE bool& getTexturesUpdated() { return m_TexturesUpdated; }
            RAZIX_INLINE void  setTexturesUpdated(bool isUpdated) { m_TexturesUpdated = isUpdated; }

            RAZIX_INLINE RZDescriptorSet* getDescriptorSet() { return m_DescriptorSet; }

            template<typename Archive>
            void save(Archive& archive) const
            {
                std::string shaderPath = "";

                if (m_Shader) {
                    std::string path = m_Shader->getPath() + m_Shader->GetName();
                    RZVirtualFileSystem::Get().absolutePathToVFS(path, shaderPath);
                }

                archive(cereal::make_nvp("Albedo", m_PBRMaterialTextures.albedo ? m_PBRMaterialTextures.albedo->getPath() : ""),
                    cereal::make_nvp("Normal", m_PBRMaterialTextures.normal ? m_PBRMaterialTextures.normal->getPath() : ""),
                    cereal::make_nvp("Metallic", m_PBRMaterialTextures.metallic ? m_PBRMaterialTextures.metallic->getPath() : ""),
                    cereal::make_nvp("Roughness", m_PBRMaterialTextures.roughness ? m_PBRMaterialTextures.roughness->getPath() : ""),
                    cereal::make_nvp("Ao", m_PBRMaterialTextures.ao ? m_PBRMaterialTextures.ao->getPath() : ""),
                    cereal::make_nvp("Emissive", m_PBRMaterialTextures.emissive ? m_PBRMaterialTextures.emissive->getPath() : ""),
                    cereal::make_nvp("albedoColor", m_MaterialProperties->albedoColor),
                    cereal::make_nvp("roughnessColor", m_MaterialProperties->roughnessColor),
                    cereal::make_nvp("metallicColor", m_MaterialProperties->metallicColor),
                    cereal::make_nvp("emissiveColor", m_MaterialProperties->emissiveColor),
                    cereal::make_nvp("isUsingAlbedoMap", m_MaterialProperties->isUsingAlbedoMap),
                    cereal::make_nvp("isUsingMetallicMap", m_MaterialProperties->isUsingMetallicMap),
                    cereal::make_nvp("isUsingRoughnessMap", m_MaterialProperties->isUsingRoughnessMap),
                    cereal::make_nvp("isUsingNormalMap", m_MaterialProperties->isUsingNormalMap),
                    cereal::make_nvp("isUsingAOMap", m_MaterialProperties->isUsingAOMap),
                    cereal::make_nvp("isUsingEmissiveMap", m_MaterialProperties->isUsingEmissiveMap),
                    cereal::make_nvp("workflow", m_MaterialProperties->workflow),
                    cereal::make_nvp("shader", shaderPath));
            }

            template<typename Archive>
            void load(Archive& archive)
            {
                std::string albedoFilePath;
                std::string normalFilePath;
                std::string roughnessFilePath;
                std::string metallicFilePath;
                std::string emissiveFilePath;
                std::string aoFilePath;
                std::string shaderFilePath;

                archive(cereal::make_nvp("Albedo", albedoFilePath),
                    cereal::make_nvp("Normal", normalFilePath),
                    cereal::make_nvp("Metallic", metallicFilePath),
                    cereal::make_nvp("Roughness", roughnessFilePath),
                    cereal::make_nvp("Ao", aoFilePath),
                    cereal::make_nvp("Emissive", emissiveFilePath),
                    cereal::make_nvp("albedoColor", m_MaterialProperties->albedoColor),
                    cereal::make_nvp("roughnessColor", m_MaterialProperties->roughnessColor),
                    cereal::make_nvp("metallicColor", m_MaterialProperties->metallicColor),
                    cereal::make_nvp("emissiveColor", m_MaterialProperties->emissiveColor),
                    cereal::make_nvp("isUsingAlbedoMap", m_MaterialProperties->isUsingAlbedoMap),
                    cereal::make_nvp("isUsingMetallicMap", m_MaterialProperties->isUsingMetallicMap),
                    cereal::make_nvp("isUsingRoughnessMap", m_MaterialProperties->isUsingRoughnessMap),
                    cereal::make_nvp("isUsingNormalMap", m_MaterialProperties->isUsingNormalMap),
                    cereal::make_nvp("isUsingAOMap", m_MaterialProperties->isUsingAOMap),
                    cereal::make_nvp("isUsingEmissiveMap", m_MaterialProperties->isUsingEmissiveMap),
                    cereal::make_nvp("workflow", m_MaterialProperties->workflow),
                    cereal::make_nvp("shader", shaderFilePath));

                if (!shaderFilePath.empty())
                    loadShader(shaderFilePath);

                if (!albedoFilePath.empty())
                    m_PBRMaterialTextures.albedo = (Graphics::Texture2D::CreateFromFile("albedo", albedoFilePath));
                if (!normalFilePath.empty())
                    m_PBRMaterialTextures.normal = (Graphics::Texture2D::CreateFromFile("roughness", normalFilePath));
                if (!metallicFilePath.empty())
                    m_PBRMaterialTextures.metallic = (Graphics::Texture2D::CreateFromFile("metallic", metallicFilePath));
                if (!roughnessFilePath.empty())
                    m_PBRMaterialTextures.roughness = (Graphics::Texture2D::CreateFromFile("roughness", roughnessFilePath));
                if (!emissiveFilePath.empty())
                    m_PBRMaterialTextures.emissive = (Graphics::Texture2D::CreateFromFile("emissive", emissiveFilePath));
                if (!aoFilePath.empty())
                    m_PBRMaterialTextures.ao = (Graphics::Texture2D::CreateFromFile("ao", aoFilePath));
            }

        private:
            static RZTexture2D* s_DefaultTexture;

            RZShader*             m_Shader;
            PBRMataterialTextures m_PBRMaterialTextures;
            RZMaterialProperties  m_MaterialProperties;
            RZUniformBuffer*      m_MaterialPropertiesUBO;
            RZDescriptorSet*      m_DescriptorSet;
            //uint32_t              m_MaterialBufferSize;
            //uint8_t*              m_MaterialBufferData;
            std::string m_Name;
            bool        m_TexturesUpdated;
        };

    }    // namespace Graphics
}    // namespace Razix
