// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMaterial.h"

#include "Razix/Graphics/API/RZShader.h"
#include "Razix/Graphics/API/RZTexture.h"
#include "Razix/Graphics/API/RZUniformBuffer.h"

namespace Razix {
    namespace Graphics {

        RZTexture2D*   RZMaterial::s_DefaultTexture = nullptr;
        MaterialPreset RZMaterial::s_MatPreset      = MaterialPreset::MAT_PRESET_FORWARD_PHONG_LIGHTING;

        RZMaterial::RZMaterial(RZShader* shader)
        {
            m_Shader = shader;

            m_MaterialPropertiesUBO = nullptr;

            // Create the uniform buffer first
            // well the type of UBO data to create depends on the shader since the pre-made shader reflection info for mat properties has to be stored somewhere
            // we store the types in the header file and use the preset to create the UBO with apt data
            if (s_MatPreset == MaterialPreset::MAT_PRESET_FORWARD_PHONG_LIGHTING)
                m_MaterialPropertiesUBO = Graphics::RZUniformBuffer::Create(sizeof(PhongMaterialProperties), &m_PhongMaterialProperties, "Material Properties UBO (Phong)");
            else if (s_MatPreset == MaterialPreset::MAT_PRESET_DEFERRED_PBR)
                m_MaterialPropertiesUBO = Graphics::RZUniformBuffer::Create(sizeof(PBRMaterialProperties), &m_PBRMaterialProperties, "Material Properties UBO (PBR)");

            // Create the descriptor sets for the shaders and Update with the apt data (user mat + render systems data)
            createDescriptorSet();
        }

        void RZMaterial::InitDefaultTexture()
        {
            uint32_t pinkTextureData = 0xff00ffff;
            s_DefaultTexture         = Graphics::RZTexture2D::Create("Default Texture", 1, 1, &pinkTextureData, RZTexture::Format::RGBA32);
        }

        void RZMaterial::ReleaseDefaultTexture()
        {
            s_DefaultTexture->Release(true);
        }

        void RZMaterial::loadMaterial(const std::string& name, const std::string& path)
        {
            m_Name = name;
            // TODO: Deserialize using the path
        }

        void RZMaterial::loadShader(const std::string& path)
        {
            // TODO: if its just a name search the VFS and ShaderLibrary of not load it into memory and do all the shit work needed thereafter
        }

        void RZMaterial::createDescriptorSet()
        {
            // Create the descriptor set for material properties data and it's textures
            // How about renderer data for forward lights info + system vars???? How to associate and update?
            auto setInfos = m_Shader->getSetsCreateInfos();
            for (auto& setInfo: setInfos) {
                if (setInfo.first == MatBindingTable_User::BINDING_SET_USER_MAT_PROPS) {
                    for (auto& descriptor: setInfo.second) {
                        // Find the material properties UBO and assign it's UBO to this slot
                        if (descriptor.bindingInfo.type == Graphics::DescriptorType::UNIFORM_BUFFER) {
                            descriptor.uniformBuffer = m_MaterialPropertiesUBO;
                        }
                    }
                } else if (setInfo.first == MatBindingTable_User::BINDING_SET_USER_MAT_SAMPLERS) {
                    for (auto& descriptor: setInfo.second) {
                        // Choose the mat textures based on the workflow & preset
                        if (s_MatPreset == MaterialPreset::MAT_PRESET_FORWARD_PHONG_LIGHTING) {
                            switch (descriptor.bindingInfo.binding) {
                                case PhongBindinngTable::PHONG_TEX_BINDING_IDX_AMBIENT:
                                    descriptor.texture = m_PhongMaterialTextures.ambient ? m_PhongMaterialTextures.ambient : s_DefaultTexture;
                                    break;
                                case PhongBindinngTable::PHONG_TEX_BINDING_IDX_DIFFUSE:
                                    descriptor.texture = m_PhongMaterialTextures.diffuse ? m_PhongMaterialTextures.diffuse : s_DefaultTexture;
                                    break;
                                case PhongBindinngTable::PHONG_TEX_BINDING_IDX_NORMAL:
                                    descriptor.texture = m_PhongMaterialTextures.normal ? m_PhongMaterialTextures.normal : s_DefaultTexture;
                                    break;
                                case PhongBindinngTable::PHONG_TEX_BINDING_IDX_SPECULAR:
                                    descriptor.texture = m_PhongMaterialTextures.specular ? m_PhongMaterialTextures.specular : s_DefaultTexture;
                                    break;
                                default:
                                    descriptor.texture = s_DefaultTexture;
                                    break;
                            }
                        } else if (s_MatPreset == MaterialPreset::MAT_PRESET_DEFERRED_PBR) {
                            if (descriptor.bindingInfo.binding == PBRBindingTable::PBR_TEX_BINDING_IDX_ALBEDO)
                                descriptor.texture = m_PBRMaterialTextures.albedo ? m_PBRMaterialTextures.albedo : s_DefaultTexture;
                            if (descriptor.bindingInfo.binding == PBRBindingTable::PBR_TEX_BINDING_IDX_NORMAL)
                                descriptor.texture = m_PBRMaterialTextures.normal ? m_PBRMaterialTextures.normal : s_DefaultTexture;
                            if (descriptor.bindingInfo.binding == PBRBindingTable::PBR_TEX_BINDING_IDX_METALLLIC)
                                descriptor.texture = m_PBRMaterialTextures.metallic ? m_PBRMaterialTextures.metallic : s_DefaultTexture;
                            if (descriptor.bindingInfo.binding == PBRBindingTable::PBR_TEX_BINDING_IDX_ROUGHNESS)
                                descriptor.texture = m_PBRMaterialTextures.roughness ? m_PBRMaterialTextures.roughness : s_DefaultTexture;
                            if (descriptor.bindingInfo.binding == PBRBindingTable::PBR_TEX_BINDING_IDX_AO)
                                descriptor.texture = m_PBRMaterialTextures.ao ? m_PBRMaterialTextures.ao : s_DefaultTexture;
                            if (descriptor.bindingInfo.binding == PBRBindingTable::PBR_TEX_BINDING_IDX_EMISSIVE)
                                descriptor.texture = m_PBRMaterialTextures.emissive ? m_PBRMaterialTextures.roughness : s_DefaultTexture;
                        }
                    }
                }
                // This holds the descriptor sets for the material Properties and Samplers
                // Now each mesh will have a material instance so each have their own sets so not a problem
                // TODO: Make sure the material instances similar to unreal exist with different Desc Sets for mat props buth with same shader instance, Simple Solution: Use a shader library to load the same shader, ofc we give the shader so that's possible
                m_DescriptorSets.push_back(RZDescriptorSet::Create(setInfo.second));
            }
        }

        void RZMaterial::setTextures(PBRMataterialTextures& textures)
        {
            m_PBRMaterialTextures.albedo    = textures.albedo;
            m_PBRMaterialTextures.normal    = textures.normal;
            m_PBRMaterialTextures.roughness = textures.roughness;
            m_PBRMaterialTextures.metallic  = textures.metallic;
            m_PBRMaterialTextures.ao        = textures.ao;
            m_PBRMaterialTextures.emissive  = textures.emissive;

            setTexturesUpdated(true);
        }

        void RZMaterial::Bind()
        {
            //  Check if the descriptor sets need to be built or updated and do that by deleting it and creating a new one
            if (m_DescriptorSets.size() || getTexturesUpdated()) {
                createDescriptorSet();
                setTexturesUpdated(false);
            }
        }

    }    // namespace Graphics
}    // namespace Razix