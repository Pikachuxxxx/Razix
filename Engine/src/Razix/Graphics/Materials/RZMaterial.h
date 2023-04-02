#pragma once

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
            glm::vec3 albedoColor         = glm::vec3(1.0f, 1.0f, 1.0f);
            bool      _padding[4]         = {};
            glm::vec3 normal              = glm::vec3(0.0f, 1.0f, 0.0f);
            //bool      _padding_[4]        = {1, 0, 1, 0};
            f32       emissiveIntensity   = 1.0f;
            f32       metallicColor       = 17.0f;
            f32       roughnessColor      = 18.0f;
            f32       specularColor       = 32.0f;
            f32       opacity             = 1.0f;
            f32       ambientOcclusion    = 111.0f;
            u32       visible             = true;
            u32       isUsingAlbedoMap    = false;
            u32       isUsingNormalMap    = false;
            u32       isUsingMetallicMap  = false;
            u32       isUsingRoughnessMap = false;
            u32       isUsingSpecular     = false;
            u32       isUsingEmissiveMap  = false;
            u32       isUsingAOMap        = false;
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

        /* Not an interface yet, this is a hard coded PBR material as of this iteration of the engine */
        class RAZIX_API RZMaterial
        {
        public:
            /* Crates the material with the given shader and it's properties */
            RZMaterial(RZShader* shader);
            virtual ~RZMaterial() {}

            void Destroy();

            /* Creates a 1x1 default pink 2D texture */
            static void InitDefaultTexture();
            /* Destroys the default texture created */
            static void         ReleaseDefaultTexture();
            static RZTexture2D* GetDefaultTexture() { return s_DefaultTexture; }
            /* Static Getter and setter for the material workflow */
            WorkFlow          getWorkflow() { return m_Workflow; }
            RAZIX_INLINE void setWorkflow(WorkFlow workflow) { m_Workflow = workflow; }

            /* Overrides the default material properties and textures by loading the material file and de-serializing it */
            void loadMaterialFromFile(const std::string& name, const std::string& path);
            void createDescriptorSet();

            RAZIX_INLINE MaterialTextures& getTextures() { return m_MaterialTextures; }
            void                           setTextures(MaterialTextures& textures);

            RAZIX_INLINE const MaterialProperties& getProperties() const { return m_MaterialProperties; }
            void                                   setProperties(MaterialProperties& props);

            void Bind();

            RAZIX_INLINE bool& getTexturesUpdated() { return m_TexturesUpdated; }
            RAZIX_INLINE void  setTexturesUpdated(bool isUpdated) { m_TexturesUpdated = isUpdated; }

            RAZIX_INLINE const std::string& getName() { return m_Name; }
            RAZIX_INLINE void               setName(const std::string& name) { m_Name = name; }

            RAZIX_INLINE RZDescriptorSet* getDescriptorSet() { return m_DescriptorSet; }

            template<typename Archive>
            void save(Archive& archive) const
            {
                std::string shaderPath = "";

                if (m_Shader) {
                    std::string path = m_Shader->getPath() + m_Shader->GetName();
                    RZVirtualFileSystem::Get().absolutePathToVFS(path, shaderPath);
                }

#if 0
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
#endif
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

#if 0
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
#endif
            }

        private:
            //RZPipeline*                   m_Pipeline; // Diffifult to be own as Material can't have knowledge of the RTs in a pass and RZPipeline needs that before hand along with shader

            static RZTexture2D* s_DefaultTexture;

            MaterialProperties m_MaterialProperties;
            MaterialTextures   m_MaterialTextures;
            std::string        m_Name                  = "PBR material";
            RZShader*          m_Shader                = nullptr;
            RZDescriptorSet*   m_DescriptorSet         = nullptr;
            RZUniformBuffer*   m_MaterialPropertiesUBO = nullptr;
            bool               m_TexturesUpdated       = false;
            WorkFlow           m_Workflow              = WorkFlow::PBR_WORKFLOW_METALLIC_ROUGHTNESS;
        };

    }    // namespace Graphics
}    // namespace Razix
