// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMaterial.h"

#include "Razix/Graphics/API/RZShader.h"
#include "Razix/Graphics/API/RZTexture.h"
#include "Razix/Graphics/API/RZUniformBuffer.h"

namespace Razix {
    namespace Graphics {

        RZTexture2D* RZMaterial::s_DefaultTexture = nullptr;

        RZMaterial::RZMaterial(RZShader* shader)
        {
            m_Shader = shader;

            m_MaterialPropertiesUBO = nullptr;

            // Create the uniform buffer first

            m_MaterialPropertiesUBO = Graphics::RZUniformBuffer::Create(sizeof(MaterialProperties), &m_MaterialProperties RZ_DEBUG_NAME_TAG_STR_E_ARG("Material Properties UBO"));
        }

        void RZMaterial::Destroy()
        {
            for (auto& set: m_DescriptorSets)
                set->Destroy();

            m_MaterialTextures.Destroy();
            if (m_MaterialPropertiesUBO)
                m_MaterialPropertiesUBO->Destroy();
        }

        void RZMaterial::InitDefaultTexture()
        {
            uint32_t pinkTextureData = 0xff00ffff;
            s_DefaultTexture         = Graphics::RZTexture2D::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Default Texture") "Default Texture", 1, 1, &pinkTextureData, RZTexture::Format::RGBA8);
        }

        void RZMaterial::ReleaseDefaultTexture()
        {
            s_DefaultTexture->Release(true);
        }

        void RZMaterial::loadMaterialFromFile(const std::string& name, const std::string& path)
        {
            m_Name = name;
            // TODO: Deserialize using the path
        }

        void RZMaterial::createDescriptorSet()
        {
            // Create the descriptor set for material properties data and it's textures
            // How about renderer data for forward lights info + system vars???? How to associate and update?
            auto setInfos = m_Shader->getSetsCreateInfos();
            for (auto& setInfo: setInfos) {
                if (setInfo.first == MatBindingTable_System::BINDING_SET_SYSTEM_MAT_PROPS) {
                    for (auto& descriptor: setInfo.second) {
                        // Find the material properties UBO and assign it's UBO to this slot
                        if (descriptor.bindingInfo.type == Graphics::DescriptorType::UNIFORM_BUFFER) {
                            descriptor.uniformBuffer = m_MaterialPropertiesUBO;
                        }
                    }
                    m_DescriptorSets.push_back(RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("BINDING_SET_USER_MAT_PROPS")));
                } else if (setInfo.first == MatBindingTable_System::BINDING_SET_SYSTEM_MAT_SAMPLERS) {
                    for (auto& descriptor: setInfo.second) {
                        // Choose the mat textures based on the workflow & preset
                        switch (descriptor.bindingInfo.binding) {
                            case TextureBindingTable::TEX_BINDING_IDX_ALBEDO:
                                descriptor.texture = m_MaterialTextures.albedo ? m_MaterialTextures.albedo : s_DefaultTexture;
                                break;
                            case TextureBindingTable::TEX_BINDING_IDX_NORMAL:
                                descriptor.texture = m_MaterialTextures.normal ? m_MaterialTextures.normal : s_DefaultTexture;
                                break;
                            case TextureBindingTable::TEX_BINDING_IDX_METALLLIC:
                                descriptor.texture = m_MaterialTextures.metallic ? m_MaterialTextures.metallic : s_DefaultTexture;
                                break;
                            case TextureBindingTable::TEX_BINDING_IDX_ROUGHNESS:
                                descriptor.texture = m_MaterialTextures.roughness ? m_MaterialTextures.roughness : s_DefaultTexture;
                                break;
                            case TextureBindingTable::TEX_BINDING_IDX_SPECULAR:
                                descriptor.texture = m_MaterialTextures.specular ? m_MaterialTextures.specular : s_DefaultTexture;
                                break;
                            case TextureBindingTable::TEX_BINDING_IDX_EMISSIVE:
                                descriptor.texture = m_MaterialTextures.emissive ? m_MaterialTextures.emissive : s_DefaultTexture;
                                break;
                            case TextureBindingTable::TEX_BINDING_IDX_AO:
                                descriptor.texture = m_MaterialTextures.ao ? m_MaterialTextures.ao : s_DefaultTexture;
                                break;
                            default:
                                descriptor.texture = s_DefaultTexture;
                                break;
                        }
                    }
                    m_DescriptorSets.push_back(RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("BINDING_SET_USER_MAT_SAMPLERS")));
                }
                // This holds the descriptor sets for the material Properties and Samplers
                // Now each mesh will have a material instance so each have their own sets so not a problem
                // TODO: Make sure the material instances similar to unreal exist with different Desc Sets for mat props both with same shader instance, Simple Solution: Use a shader library to load the same shader, of course we give the shader so that's possible
            }
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
            memcpy(&m_MaterialProperties, &props, sizeof(MaterialProperties));
            m_MaterialPropertiesUBO->SetData(sizeof(MaterialProperties), &m_MaterialProperties);
        }

        void RZMaterial::Bind()
        {
            //  Check if the descriptor sets need to be built or updated and do that by deleting it and creating a new one
            if (!m_DescriptorSets.size() || getTexturesUpdated()) {
                for (auto& descset: m_DescriptorSets)
                    descset->Destroy();
                m_DescriptorSets.clear();
                createDescriptorSet();
                setTexturesUpdated(false);
            }
            // Since we need to bind all the sets at once IDK about using bind, how does the mat get the Render System Descriptors to bind???
            // This possible if do something like Unity does, have a Renderer Component for every renderable entity in the scene ==> this makes
            // it easy for get info about Culling too, using this we can easily get the System Sets and Bind them
            // For now since we use the same shader we can just let the renderer Bind it and the material will give the Renderer necessary Sets to bind
        }

        void MaterialTextures::Destroy()
        {
            if (albedo && albedo != RZMaterial::GetDefaultTexture())
                albedo->Release(true);
            if (normal && normal != RZMaterial::GetDefaultTexture())
                normal->Release(true);
            if (metallic && metallic != RZMaterial::GetDefaultTexture())
                metallic->Release(true);
            if (roughness && roughness != RZMaterial::GetDefaultTexture())
                roughness->Release(true);
            if (specular && specular != RZMaterial::GetDefaultTexture())
                specular->Release(true);
            if (emissive && emissive != RZMaterial::GetDefaultTexture())
                emissive->Release(true);
            if (ao && ao != RZMaterial::GetDefaultTexture())
                ao->Release(true);
        }
    }    // namespace Graphics
}    // namespace Razix