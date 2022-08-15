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

        RZMaterial::RZMaterial(RZShader* shader, RZMaterialProperties matProps /*= RZMaterialProperties()*/, PBRMataterialTextures textures /*= PBRMataterialTextures()*/)
        {
            m_Shader              = shader;
            m_MaterialProperties  = matProps;
            m_PBRMaterialTextures = textures;

            m_MaterialPropertiesUBO = nullptr;
            //m_MaterialBufferSize       = sizeof(RZMaterialProperties);
            //m_MaterialBufferData       = new uint8_t[m_MaterialBufferSize];

            // Create the uniform buffer first
            m_MaterialPropertiesUBO = Graphics::RZUniformBuffer::Create(sizeof(RZMaterialProperties), &m_MaterialProperties, "Material Properties UBO");
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
            m_Name                = name;
            m_PBRMaterialTextures = PBRMataterialTextures();
        }

        void RZMaterial::loadShader(const std::string& path)
        {
        }

        void RZMaterial::createDescriptorSet()
        {
            // TODO: Make this more intuitive and less dirty like this!!!!
            // Create the descriptor set for material properties data and it's textures
            auto setInfos      = m_Shader->getSetsCreateInfos();
            int  texBindingidx = 0;
            for (auto& setInfo: setInfos) {
                if (setInfo.first == SYSTEM_SET_INDEX_MATERIAL_DATA) {
                    for (auto& descriptor: setInfo.second) {
                        // Find the material properties UBO and assign it's UBO to this slot
                        if (setInfo.first == SYSTEM_SET_INDEX_MATERIAL_DATA && descriptor.bindingInfo.type == Graphics::DescriptorType::UNIFORM_BUFFER) {
                            descriptor.uniformBuffer = m_MaterialPropertiesUBO;
                        } else if (setInfo.first == SYSTEM_SET_INDEX_MATERIAL_DATA && descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER) {
                            if (texBindingidx == 0)
                                descriptor.texture = m_PBRMaterialTextures.albedo ? m_PBRMaterialTextures.albedo : s_DefaultTexture;
                            if (texBindingidx == 1)
                                descriptor.texture = m_PBRMaterialTextures.normal ? m_PBRMaterialTextures.normal : s_DefaultTexture;
                            if (texBindingidx == 2)
                                descriptor.texture = m_PBRMaterialTextures.metallic ? m_PBRMaterialTextures.metallic : s_DefaultTexture;
                            if (texBindingidx == 3)
                                descriptor.texture = m_PBRMaterialTextures.roughness ? m_PBRMaterialTextures.roughness : s_DefaultTexture;
                            if (texBindingidx == 4)
                                descriptor.texture = m_PBRMaterialTextures.ao ? m_PBRMaterialTextures.ao : s_DefaultTexture;
                            if (texBindingidx == 5)
                                descriptor.texture = m_PBRMaterialTextures.emissive ? m_PBRMaterialTextures.roughness : s_DefaultTexture;
                            texBindingidx++;
                        }
                    }
                    // This holds the descriptor set only for the Material Index
                    m_DescriptorSet = RZDescriptorSet::Create(setInfo.second);
                }
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
            if (m_DescriptorSet == nullptr || getTexturesUpdated()) {
                createDescriptorSet();
                setTexturesUpdated(false);
            }
        }

    }    // namespace Graphics
}    // namespace Razix