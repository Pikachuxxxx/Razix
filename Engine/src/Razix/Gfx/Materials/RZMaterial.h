#pragma once

#include "Razix/Gfx/Materials/RZMaterialData.h"

namespace Razix {
    namespace Gfx {

        constexpr cstr kMaterialAssetFilePrefix = "MT_";

        /* Not an interface yet, this is a hard coded PBR material as of this iteration of the engine */
        class RAZIX_API RZMaterial
        {
        public:
            RZMaterial(RZShaderHandle shader);
            ~RZMaterial() {}

            void Destroy();

            /* Creates a 1x1 default pink 2D texture */
            static void            InitDefaultTexture();
            static void            ReleaseDefaultTexture();
            static RZTextureHandle GetDefaultTexture() { return s_DefaultTexture; }
            static RZMaterial*     GetDefaultMaterial();

            void Bind(RZDrawCommandBufferHandle cmdBuffer = {});

            void loadFromFile(const std::string& path);
            void saveToFile(const std::string& path = "");
            void loadMaterialTexturesFromFiles(MaterialTexturePaths paths);
            void createDescriptorSet();
            
            void                        setProperties(MaterialProperties& props);
            void                        setTexturePaths(MaterialTexturePaths& paths);
            
            inline std::string                 getName() { return m_MaterialData.m_Name; }
            inline void                        setName(const std::string& name) { strcpy(m_MaterialData.m_Name, name.c_str()); }
            inline const MaterialTextures&     getTextures() const { return m_MaterialData.m_MaterialTextures; }
            inline void                        setTextures(MaterialTextures& textures);
            inline const MaterialProperties&   getProperties() const { return m_MaterialData.m_MaterialProperties; }
            inline const MaterialTexturePaths& getTexturePaths() const { return m_MaterialData.m_MaterialTexturePaths; }
            inline bool                        getTexturesUpdated() const { return m_TexturesUpdated; }
            inline void                        setTexturesUpdated(bool isUpdated) { m_TexturesUpdated = isUpdated; }
            inline RZDescriptorSetHandle       getDescriptorSet() { return m_DescriptorSet; }
            inline WorkFlow                    getWorkflow() const { return (Gfx::WorkFlow) m_MaterialData.m_MaterialProperties.workflow; }
            inline void                        setWorkflow(WorkFlow workflow) { m_MaterialData.m_MaterialProperties.workflow = (u32) workflow; }

        private:
            static RZTextureHandle s_DefaultTexture;
            static RZMaterial*     s_DefaultMaterial;

            MaterialData          m_MaterialData          = {};
            RZShaderHandle        m_Shader                = {};
            RZPipelineHandle      m_Pipeline              = {};
            RZDescriptorSetHandle m_DescriptorSet         = {};
            RZUniformBufferHandle m_MaterialPropertiesUBO = {};
            bool                  m_TexturesUpdated       = false;
        };
    }    // namespace Gfx
}    // namespace Razix
