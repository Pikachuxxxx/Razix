// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMaterial.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Utilities/RZStringUtilities.h"

#include <cereal/archives/json.hpp>

namespace Razix {
    namespace Graphics {

        RZTextureHandle RZMaterial::s_DefaultTexture;

        RZMaterial::RZMaterial(RZShaderHandle shader)
        {
            m_Shader = shader;

            // Create the uniform buffer first
            m_MaterialPropertiesUBO = RZResourceManager::Get().createUniformBuffer({"Material Properties UBO", sizeof(MaterialProperties), &m_MaterialData.m_MaterialProperties});
        }

        void RZMaterial::Destroy()
        {
            if (m_DescriptorSet)
                m_DescriptorSet->Destroy();

            m_MaterialTextures.Destroy();
            RZResourceManager::Get().destroyUniformBuffer(m_MaterialPropertiesUBO);
        }

        void RZMaterial::InitDefaultTexture()
        {
            //u32  pinkTextureData    = 0xffff00ff;    // A8B8G8R8
            u32* pinkTextureDataRaw = new u32;    // A8B8G8R8
            pinkTextureDataRaw[0]   = {0xffff00ff};
            //memcpy(pinkTextureDataRaw, &pinkTextureData, sizeof(u32));
            s_DefaultTexture = RZResourceManager::Get().createTexture({.name = "Default Texture", .width = 1, .height = 1, .data = pinkTextureDataRaw, .format = TextureFormat::RGBA8});
            //delete[] pinkTextureDataRaw;
        }

        void RZMaterial::ReleaseDefaultTexture()
        {
            RZResourceManager::Get().destroyTexture(s_DefaultTexture);
        }

        void RZMaterial::loadFromFile(const std::string& path)
        {
            m_Name = Utilities::RemoveFilePathExtension(Utilities::GetFileName(path));
            // Using the Name of the Material search the //Assets/Materials + MeshName_MaterialName.rzmaterial file and load it and set the material Data
            Razix::Graphics::MaterialData matData{};
            std::string                   matPhysicalPath;
            if (RZVirtualFileSystem::Get().resolvePhysicalPath(path, matPhysicalPath)) {
                if (!matPhysicalPath.empty()) {
                    std::ifstream AppStream;
                    AppStream.open(matPhysicalPath, std::ifstream::in);
                    cereal::JSONInputArchive inputArchive(AppStream);

                    inputArchive(cereal::make_nvp("$MATERIAL", matData));
                }
            }
            setProperties(matData.m_MaterialProperties);
            loadMaterialTexturesFromFiles(matData.m_MaterialTexturePaths);
            //createDescriptorSet();
        }

        void RZMaterial::saveToFile(const std::string& path)
        {
            if (path.empty()) {
                // Just resolve the folder path to save to
                std::string matPath = "//Assets/Materials/";
                std::string matPhysicalPath;
                if (RZVirtualFileSystem::Get().resolvePhysicalPath(matPath, matPhysicalPath, true)) {
                    matPhysicalPath += std::string(m_Name) + ".rzmaterial";
                    std::ofstream             opAppStream(matPhysicalPath);
                    cereal::JSONOutputArchive defArchive(opAppStream);
                    defArchive(cereal::make_nvp("$MATERIAL", m_MaterialData));
                }
            } else {
                std::string matPhysicalPath;
                if (RZVirtualFileSystem::Get().resolvePhysicalPath(path, matPhysicalPath)) {
                    std::ofstream             opAppStream(matPhysicalPath);
                    cereal::JSONOutputArchive defArchive(opAppStream);
                    defArchive(cereal::make_nvp("$MATERIAL", m_MaterialData));
                }
            }
        }

        void RZMaterial::loadMaterialTexturesFromFiles(MaterialTexturePaths paths)
        {
            m_MaterialData.m_MaterialTexturePaths = paths;

            if (!std::string(paths.albedo).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.albedo);
                if (!fileName.empty())
                    m_MaterialTextures.albedo = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.albedo);
            }

            if (!std::string(paths.ao).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.ao);
                if (!fileName.empty())
                    m_MaterialTextures.ao = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.ao);
            }

            if (!std::string(paths.emissive).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.emissive);
                if (!fileName.empty())
                    m_MaterialTextures.emissive = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.emissive);
            }

            if (!std::string(paths.metallic).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.metallic);
                if (!fileName.empty())
                    m_MaterialTextures.metallic = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.metallic);
            }

            if (!std::string(paths.normal).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.normal);
                if (!fileName.empty())
                    m_MaterialTextures.normal = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.normal);
            }

            if (!std::string(paths.roughness).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.roughness);
                if (!fileName.empty())
                    m_MaterialTextures.roughness = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.roughness);
            }

            if (!std::string(paths.specular).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.specular);
                if (!fileName.empty())
                    m_MaterialTextures.specular = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.specular);
            }

            setTexturesUpdated(true);
        }

        void RZMaterial::createDescriptorSet()
        {
#if 1
            // Create the descriptor set for material properties data and it's textures
            // How about renderer data for forward lights info + system vars???? How to associate and update?
            auto setInfos = RZResourceManager::Get().getShaderResource(m_Shader)->getDescriptorsPerHeapMap();
            for (auto& setInfo: setInfos) {
                if (setInfo.first == BindingTable_System::SET_IDX_MATERIAL_DATA) {
                    for (auto& descriptor: setInfo.second) {
                        // Find the material properties UBO and assign it's UBO to this slot
                        if (descriptor.bindingInfo.type == Graphics::DescriptorType::UniformBuffer) {
                            descriptor.uniformBuffer = m_MaterialPropertiesUBO;
                        }
    #if 1
                        else if (descriptor.bindingInfo.type == Graphics::DescriptorType::ImageSamplerCombined) {
                            // Choose the mat textures based on the workflow & preset
                            switch (descriptor.bindingInfo.location.binding) {
                                case TextureBindingTable::BINDING_IDX_TEX_ALBEDO:
                                    descriptor.texture = m_MaterialTextures.albedo.isValid() ? m_MaterialTextures.albedo : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingAlbedoMap = true;
                                    break;
                                case TextureBindingTable::BINDING_IDX_TEX_NORMAL:
                                    descriptor.texture = m_MaterialTextures.normal.isValid() ? m_MaterialTextures.normal : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingNormalMap = true;
                                    break;
                                case TextureBindingTable::BINDING_IDX_TEX_METALLLIC:
                                    descriptor.texture = m_MaterialTextures.metallic.isValid() ? m_MaterialTextures.metallic : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingMetallicMap = true;
                                    break;
                                case TextureBindingTable::BINDING_IDX_TEX_ROUGHNESS:
                                    descriptor.texture = m_MaterialTextures.roughness.isValid() ? m_MaterialTextures.roughness : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingRoughnessMap = true;
                                    break;
                                case TextureBindingTable::BINDING_IDX_TEX_SPECULAR:
                                    descriptor.texture = m_MaterialTextures.specular.isValid() ? m_MaterialTextures.specular : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingSpecular = true;
                                    break;
                                case TextureBindingTable::BINDING_IDX_TEX_EMISSIVE:
                                    descriptor.texture = m_MaterialTextures.emissive.isValid() ? m_MaterialTextures.emissive : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingEmissiveMap = true;
                                    break;
                                case TextureBindingTable::BINDING_IDX_TEX_AO:
                                    descriptor.texture = m_MaterialTextures.ao.isValid() ? m_MaterialTextures.ao : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingAOMap = true;
                                    break;
                                default:
                                    descriptor.texture = s_DefaultTexture;
                                    break;
                            }
                        }
    #endif
                    }
                    m_DescriptorSet = RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("BINDING_SET_MAT_PROPS"));
                }
                // This holds the descriptor sets for the material Properties and Samplers
                // Now each mesh will have a material instance so each have their own sets so not a problem
                // TODO: Make sure the material instances similar to unreal exist with different Desc Sets for mat props both with same shader instance, Simple Solution: Use a shader library to load the same shader, of course we give the shader so that's possible
            }
#endif
            // TODO!!: Support more workflows, or read it from material file itself! IMPORVE THIS!!!
            if (m_MaterialData.m_MaterialProperties.isUsingMetallicMap && !m_MaterialData.m_MaterialProperties.isUsingRoughnessMap && !m_MaterialData.m_MaterialProperties.isUsingAOMap)
                m_MaterialData.m_MaterialProperties.workflow = (u32) WorkFlow::WORLFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED;
            else
                m_MaterialData.m_MaterialProperties.workflow = (u32) WorkFlow::WORLFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE;

            // Update the properties buffer
            setProperties(m_MaterialData.m_MaterialProperties);
        }

        void RZMaterial::setTextures(MaterialTextures& textures)
        {
            m_MaterialTextures.albedo    = textures.albedo;
            m_MaterialTextures.normal    = textures.normal;
            m_MaterialTextures.metallic  = textures.metallic;
            m_MaterialTextures.roughness = textures.roughness;
            m_MaterialTextures.specular  = textures.specular;
            m_MaterialTextures.emissive  = textures.emissive;
            m_MaterialTextures.ao        = textures.ao;

            setTexturesUpdated(true);
        }

        void RZMaterial::setProperties(MaterialProperties& props)
        {
            memcpy(&m_MaterialData.m_MaterialProperties, &props, sizeof(MaterialProperties));
            auto materialBuffer = RZResourceManager::Get().getUniformBufferResource(m_MaterialPropertiesUBO);
            materialBuffer->SetData(sizeof(MaterialProperties), &m_MaterialData.m_MaterialProperties);
        }

        void RZMaterial::setTexturePaths(MaterialTexturePaths& paths)
        {
            memcpy(&m_MaterialData.m_MaterialTexturePaths, &paths, sizeof(paths));
        }

        void RZMaterial::Bind(RZPipeline* pipeline /*= nullptr*/, RZCommandBuffer* cmdBuffer /*= nullptr*/)
        {
            //  Check if the descriptor sets need to be built or updated and do that by deleting it and creating a new one
            if (!m_DescriptorSet || getTexturesUpdated()) {
                if (m_DescriptorSet)
                    m_DescriptorSet->Destroy();

                createDescriptorSet();
                setTexturesUpdated(false);

                m_MaterialData.m_MaterialProperties.AlbedoMapIdx    = m_MaterialTextures.albedo.isValid() ? m_MaterialTextures.albedo.getIndex() : s_DefaultTexture.getIndex();
                m_MaterialData.m_MaterialProperties.NormalMapIdx    = m_MaterialTextures.normal.isValid() ? m_MaterialTextures.normal.getIndex() : s_DefaultTexture.getIndex();
                m_MaterialData.m_MaterialProperties.MetallicMapIdx  = m_MaterialTextures.metallic.isValid() ? m_MaterialTextures.metallic.getIndex() : s_DefaultTexture.getIndex();
                m_MaterialData.m_MaterialProperties.RoughnessMapIdx = m_MaterialTextures.roughness.isValid() ? m_MaterialTextures.roughness.getIndex() : s_DefaultTexture.getIndex();
                m_MaterialData.m_MaterialProperties.SpecularIdx     = m_MaterialTextures.specular.isValid() ? m_MaterialTextures.specular.getIndex() : s_DefaultTexture.getIndex();
                m_MaterialData.m_MaterialProperties.EmissiveMapIdx  = m_MaterialTextures.emissive.isValid() ? m_MaterialTextures.emissive.getIndex() : s_DefaultTexture.getIndex();
                m_MaterialData.m_MaterialProperties.AOMapIdx        = m_MaterialTextures.ao.isValid() ? m_MaterialTextures.ao.getIndex() : s_DefaultTexture.getIndex();

                // Since the mat props have been updated regarding isTextureAvailable or not we need to update the UBO data again
                setProperties(m_MaterialData.m_MaterialProperties);
            }
            // Since we need to bind all the sets at once IDK about using bind, how does the mat get the Render System Descriptors to bind???
            // This possible if do something like Unity does, have a Renderer Component for every renderable entity in the scene ==> this makes
            // it easy for get info about Culling too, using this we can easily get the System Sets and Bind them
            // For now since we use the same shader we can just let the renderer Bind it and the material will give the Renderer necessary Sets to bind

            // [BINDLESS TEST]
            // Master Set must be set by RHI before drawing the scene
            // Upload the texture indices using PushConstants as MaterialTexturesData
            //struct alignas(16) PCMaterialTexturesData
            //{
            //    MaterialProperties material  = {};
            //    int                albedo    = u16_max;
            //    int                normal    = u16_max;
            //    int                metallic  = u16_max;
            //    int                roughness = u16_max;
            //    int                specular  = u16_max;
            //    int                emissive  = u16_max;
            //    int                ao        = u16_max;
            //    int                dummy     = u16_max;
            //    int                dummy2    = u16_max;
            //} data{};
            //RZPushConstant pc;
            //pc.shaderStage = ShaderStage::PIXEL;
            //pc.offset      = 0;
            //pc.size        = sizeof(PCMaterialTexturesData);
            //pc.data        = &data;

            //Graphics::RHI::BindPushConstant(pipeline, cmdBuffer, pc);
        }

        void MaterialTextures::Destroy()
        {
            if (albedo.isValid() && albedo != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().destroyTexture(albedo);
            if (normal.isValid() && normal != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().destroyTexture(normal);
            if (metallic.isValid() && metallic != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().destroyTexture(metallic);
            if (roughness.isValid() && roughness != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().destroyTexture(roughness);
            if (specular.isValid() && specular != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().destroyTexture(specular);
            if (emissive.isValid() && emissive != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().destroyTexture(emissive);
            if (ao.isValid() && ao != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().destroyTexture(ao);
        }

        RZMaterial* GetDefaultMaterial()
        {
            if (DefaultMaterial == nullptr) {
                auto shader     = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);
                DefaultMaterial = new RZMaterial(shader);
                DefaultMaterial->setName("DefaultMaterial");
                Razix::Graphics::MaterialData matData{};
                DefaultMaterial->setProperties(matData.m_MaterialProperties);
                DefaultMaterial->loadMaterialTexturesFromFiles(matData.m_MaterialTexturePaths);
                //DefaultMaterial->createDescriptorSet();
                return DefaultMaterial;
            } else
                return DefaultMaterial;
        }
    }    // namespace Graphics
}    // namespace Razix