// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMaterial.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Gfx/RHI/API/RZShader.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/Renderers/RZSystemBinding.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Utilities/RZStringUtilities.h"

#include <cereal/archives/json.hpp>

namespace Razix {
    namespace Gfx {

        RZTextureHandle RZMaterial::s_DefaultTexture  = {};
        RZMaterial*     RZMaterial::s_DefaultMaterial = nullptr;

        RZMaterial::RZMaterial(RZShaderHandle shader)
        {
            m_Shader = shader;

            // Create the uniform buffer first, we hardly change material props that too too frequently to event have it as Staging so it's fine to have it dynamic
            RZBufferDesc matBufferDesc          = {};
            matBufferDesc.name                  = "CB_Material.Props";
            matBufferDesc.usage                 = BufferUsage::PersistentStream;
            matBufferDesc.size                  = sizeof(MaterialProperties);
            matBufferDesc.data                  = &m_MaterialData.m_MaterialProperties;
            matBufferDesc.initResourceViewHints = ResourceViewHint::kCBV;
            m_MaterialPropertiesUBO             = RZResourceManager::Get().createUniformBuffer(matBufferDesc);
        }

        void RZMaterial::Destroy()
        {
            m_MaterialData.m_MaterialTextures.Destroy();
            RZResourceManager::Get().destroyUniformBuffer(m_MaterialPropertiesUBO);
            RZResourceManager::Get().destroyDescriptorSet(m_DescriptorSet);
        }

        void RZMaterial::InitDefaultTexture()
        {
            u8* pinkTextureDataRaw = new u8[4];    // A8B8G8R8
            pinkTextureDataRaw[0]  = 0xff;
            pinkTextureDataRaw[1]  = 0x00;
            pinkTextureDataRaw[2]  = 0xff;
            pinkTextureDataRaw[3]  = 0xff;

            RZTextureDesc pinkDefDesc{};
            pinkDefDesc.name   = "Texture.Builtin.Default.Pink.1x1";
            pinkDefDesc.width  = 1;
            pinkDefDesc.height = 1;
            pinkDefDesc.data   = pinkTextureDataRaw;
            pinkDefDesc.size   = sizeof(u8) * 4;
            pinkDefDesc.format = TextureFormat::RGBA8;
            s_DefaultTexture   = RZResourceManager::Get().createTexture(pinkDefDesc);
        }

        void RZMaterial::ReleaseDefaultTexture()
        {
            RZResourceManager::Get().destroyTexture(s_DefaultTexture);
        }

        RZMaterial* RZMaterial::GetDefaultMaterial()
        {
            if (s_DefaultMaterial == nullptr) {
                auto shader       = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);
                s_DefaultMaterial = new RZMaterial(shader);
                return s_DefaultMaterial;
            } else
                return s_DefaultMaterial;
        }

        void RZMaterial::loadFromFile(const std::string& path)
        {
            auto m_Name = Utilities::RemoveFilePathExtension(Utilities::GetFileName(path));
            setName(m_Name);
            // Using the Name of the Material search the //Assets/Materials + MeshName_MaterialName.rzmaterial file and load it and set the material Data
            Razix::Gfx::MaterialData matData{};
            std::string              matPhysicalPath;
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
                    matPhysicalPath += std::string(m_MaterialData.m_Name) + ".rzmaterial";
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

            RZTextureDesc desc = {};

            if (!std::string(paths.albedo).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.albedo);
                desc.name     = fileName;
                desc.filePath = paths.albedo;
                if (!fileName.empty())
                    m_MaterialData.m_MaterialTextures.albedo = RZResourceManager::Get().createTexture(desc);
            }

            if (!std::string(paths.ao).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.ao);
                desc.name     = fileName;
                desc.filePath = paths.ao;
                if (!fileName.empty())
                    m_MaterialData.m_MaterialTextures.ao = RZResourceManager::Get().createTexture(desc);
            }

            if (!std::string(paths.emissive).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.emissive);
                desc.name     = fileName;
                desc.filePath = paths.emissive;
                if (!fileName.empty())
                    m_MaterialData.m_MaterialTextures.emissive = RZResourceManager::Get().createTexture(desc);
            }

            if (!std::string(paths.metallic).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.metallic);
                desc.name     = fileName;
                desc.filePath = paths.metallic;
                if (!fileName.empty())
                    m_MaterialData.m_MaterialTextures.metallic = RZResourceManager::Get().createTexture(desc);
            }

            if (!std::string(paths.normal).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.normal);
                desc.name     = fileName;
                desc.filePath = paths.normal;
                if (!fileName.empty())
                    m_MaterialData.m_MaterialTextures.normal = RZResourceManager::Get().createTexture(desc);
            }

            if (!std::string(paths.roughness).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.roughness);
                desc.name     = fileName;
                desc.filePath = paths.roughness;
                if (!fileName.empty())
                    m_MaterialData.m_MaterialTextures.roughness = RZResourceManager::Get().createTexture(desc);
            }

            if (!std::string(paths.specular).empty()) {
                auto fileName = Razix::Utilities::GetFileName(paths.specular);
                desc.name     = fileName;
                desc.filePath = paths.specular;
                if (!fileName.empty())
                    m_MaterialData.m_MaterialTextures.specular = RZResourceManager::Get().createTexture(desc);
            }

            setTexturesUpdated(true);
        }

        void RZMaterial::createDescriptorSet()
        {
            // Create the descriptor set for material properties data and it's textures
            // How about renderer data for forward lights info + system vars???? How to associate and update?
            auto descriptorHeapsMap = RZResourceManager::Get().getShaderResource(m_Shader)->getDescriptorsPerHeapMap();
            for (auto& heap: descriptorHeapsMap) {
                if (heap.first == BindingTable_System::SET_IDX_MATERIAL_DATA) {
                    for (auto& descriptor: heap.second) {
                        // Find the material properties UBO and assign it's UBO to this slot
                        if (descriptor.bindingInfo.type == Gfx::DescriptorType::kUniformBuffer) {
                            descriptor.uniformBuffer = m_MaterialPropertiesUBO;
                        } else if (descriptor.bindingInfo.type == Gfx::DescriptorType::kImageSamplerCombined) {
                            // Choose the mat textures based on the workflow & preset
                            switch (descriptor.bindingInfo.location.binding) {
                                case TextureBindingTable::BINDING_IDX_TEX_ALBEDO:
                                    descriptor.texture = m_MaterialData.m_MaterialTextures.albedo.isValid() ? m_MaterialData.m_MaterialTextures.albedo : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingAlbedoMap = true;
                                    break;
                                case TextureBindingTable::BINDING_IDX_TEX_NORMAL:
                                    descriptor.texture = m_MaterialData.m_MaterialTextures.normal.isValid() ? m_MaterialData.m_MaterialTextures.normal : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingNormalMap = true;
                                    break;
                                case TextureBindingTable::BINDING_IDX_TEX_METALLLIC:
                                    descriptor.texture = m_MaterialData.m_MaterialTextures.metallic.isValid() ? m_MaterialData.m_MaterialTextures.metallic : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingMetallicMap = true;
                                    break;
                                case TextureBindingTable::BINDING_IDX_TEX_ROUGHNESS:
                                    descriptor.texture = m_MaterialData.m_MaterialTextures.roughness.isValid() ? m_MaterialData.m_MaterialTextures.roughness : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingRoughnessMap = true;
                                    break;
                                case TextureBindingTable::BINDING_IDX_TEX_SPECULAR:
                                    descriptor.texture = m_MaterialData.m_MaterialTextures.specular.isValid() ? m_MaterialData.m_MaterialTextures.specular : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingSpecular = true;
                                    break;
                                case TextureBindingTable::BINDING_IDX_TEX_EMISSIVE:
                                    descriptor.texture = m_MaterialData.m_MaterialTextures.emissive.isValid() ? m_MaterialData.m_MaterialTextures.emissive : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingEmissiveMap = true;
                                    break;
                                case TextureBindingTable::BINDING_IDX_TEX_AO:
                                    descriptor.texture = m_MaterialData.m_MaterialTextures.ao.isValid() ? m_MaterialData.m_MaterialTextures.ao : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingAOMap = true;
                                    break;
                                default:
                                    descriptor.texture = s_DefaultTexture;
                                    break;
                            }
                        }
                    }
                    RZDescriptorSetDesc descSetCreateDesc = {};
                    descSetCreateDesc.heapType            = DescriptorHeapType::kCbvUavSrvHeap;
                    descSetCreateDesc.name                = "DescriptorSet.Material";
                    descSetCreateDesc.descriptors         = heap.second;
                    m_DescriptorSet                       = RZResourceManager::Get().createDescriptorSet(descSetCreateDesc);
                }
                // This holds the descriptor sets for the material Properties and Samplers
                // Now each mesh will have a material instance so each have their own sets so not a problem
                // TODO: Make sure the material instances similar to unreal exist with different Desc Sets for mat props both with same shader instance, Simple Solution: Use a shader library to load the same shader, of course we give the shader so that's possible
            }
            // TODO!!: Support more workflows, or read it from material file itself! IMPORVE THIS!!!
            if (m_MaterialData.m_MaterialProperties.isUsingMetallicMap && !m_MaterialData.m_MaterialProperties.isUsingRoughnessMap && !m_MaterialData.m_MaterialProperties.isUsingAOMap)
                m_MaterialData.m_MaterialProperties.workflow = (u32) WorkFlow::WORKFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED;
            else
                m_MaterialData.m_MaterialProperties.workflow = (u32) WorkFlow::WORKFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE;

            // Update the properties buffer
            setProperties(m_MaterialData.m_MaterialProperties);
        }

        void RZMaterial::setTextures(MaterialTextures& textures)
        {
            m_MaterialData.m_MaterialTextures.albedo    = textures.albedo;
            m_MaterialData.m_MaterialTextures.normal    = textures.normal;
            m_MaterialData.m_MaterialTextures.metallic  = textures.metallic;
            m_MaterialData.m_MaterialTextures.roughness = textures.roughness;
            m_MaterialData.m_MaterialTextures.specular  = textures.specular;
            m_MaterialData.m_MaterialTextures.emissive  = textures.emissive;
            m_MaterialData.m_MaterialTextures.ao        = textures.ao;

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

        void RZMaterial::Bind(RZPipeline* pipeline /*= nullptr*/, RZDrawCommandBufferHandle cmdBuffer /*= nullptr*/)
        {
            //  Check if the descriptor sets need to be built or updated and do that by deleting it and creating a new one
            if (!m_DescriptorSet.isValid() || getTexturesUpdated()) {
                RZResourceManager::Get().destroyDescriptorSet(m_DescriptorSet);

                createDescriptorSet();
                setTexturesUpdated(false);

                m_MaterialData.m_MaterialProperties.AlbedoMapIdx    = m_MaterialData.m_MaterialTextures.albedo.isValid() ? m_MaterialData.m_MaterialTextures.albedo.getIndex() : s_DefaultTexture.getIndex();
                m_MaterialData.m_MaterialProperties.NormalMapIdx    = m_MaterialData.m_MaterialTextures.normal.isValid() ? m_MaterialData.m_MaterialTextures.normal.getIndex() : s_DefaultTexture.getIndex();
                m_MaterialData.m_MaterialProperties.MetallicMapIdx  = m_MaterialData.m_MaterialTextures.metallic.isValid() ? m_MaterialData.m_MaterialTextures.metallic.getIndex() : s_DefaultTexture.getIndex();
                m_MaterialData.m_MaterialProperties.RoughnessMapIdx = m_MaterialData.m_MaterialTextures.roughness.isValid() ? m_MaterialData.m_MaterialTextures.roughness.getIndex() : s_DefaultTexture.getIndex();
                m_MaterialData.m_MaterialProperties.SpecularIdx     = m_MaterialData.m_MaterialTextures.specular.isValid() ? m_MaterialData.m_MaterialTextures.specular.getIndex() : s_DefaultTexture.getIndex();
                m_MaterialData.m_MaterialProperties.EmissiveMapIdx  = m_MaterialData.m_MaterialTextures.emissive.isValid() ? m_MaterialData.m_MaterialTextures.emissive.getIndex() : s_DefaultTexture.getIndex();
                m_MaterialData.m_MaterialProperties.AOMapIdx        = m_MaterialData.m_MaterialTextures.ao.isValid() ? m_MaterialData.m_MaterialTextures.ao.getIndex() : s_DefaultTexture.getIndex();

                // Since the mat props have been updated regarding isTextureAvailable or not we need to update the UBO data again
                setProperties(m_MaterialData.m_MaterialProperties);
            }
            // Since we need to bind all the sets at once IDK about using bind, how does the mat get the Render System Descriptors to bind???
            // This possible if do something like Unity does, have a Renderer Component for every renderable entity in the scene ==> this makes
            // it easy for get info about Culling too, using this we can easily get the System Sets and Bind them
            // For now since we use the same shader we can just let the renderer Bind it and the material will give the Renderer necessary Sets to bind
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
    }    // namespace Gfx
}    // namespace Razix
