// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPBRMaterial.h"

#include "Razix/Graphics/API/RZShader.h"
#include "Razix/Graphics/API/RZTexture.h"

namespace Razix {
    namespace Graphics {

        RZTexture2D* RZPBRMaterial::s_DefaultTexture = nullptr;

        void RZPBRMaterial::InitDefaultTexture()
        {
        }

        void RZPBRMaterial::ReleaseDefaultTexture()
        {
        }

        void RZPBRMaterial::create(RZShader* shader, RZMaterialProperties matProps /*= RZMaterialProperties()*/, PBRMataterialTextures textures /*= PBRMataterialTextures()*/)
        {
            m_Shader              = shader;
            m_MaterialProperties  = matProps;
            m_PBRMaterialTextures = textures;

            m_MaterialPropertiesBuffer = nullptr;
            m_MaterialBufferSize       = sizeof(RZMaterialProperties);
            m_MaterialBufferData       = new uint8_t[m_MaterialBufferSize];

            // Create the uniform buffer first
            //
            // Create the descriptor set
        }

        void RZPBRMaterial::loadMaterial(const std::string& name, const std::string& path)
        {
            m_Name                = name;
            m_PBRMaterialTextures = PBRMataterialTextures();

            /*
            auto filePath = path + "/" + name + "/albedo" + extension;

            if (FileExists(filePath))
                m_PBRMaterialTextures.albedo = Ref<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path + "/" + name + "/albedo" + extension, params));

            filePath = path + "/" + name + "/normal" + extension;

            if (FileExists(filePath))
                m_PBRMaterialTextures.normal = Ref<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path + "/" + name + "/normal" + extension, params));

            filePath = path + "/" + name + "/roughness" + extension;

            if (FileExists(filePath))
                m_PBRMaterialTextures.roughness = Ref<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path + "/" + name + "/roughness" + extension, params));

            filePath = path + "/" + name + "/metallic" + extension;

            if (FileExists(filePath))
                m_PBRMaterialTextures.metallic = Ref<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path + "/" + name + "/metallic" + extension, params));

            filePath = path + "/" + name + "/ao" + extension;

            if (FileExists(filePath))
                m_PBRMaterialTextures.ao = Ref<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path + "/" + name + "/ao" + extension, params));

            filePath = path + "/" + name + "/emissive" + extension;

            if (FileExists(filePath))
                m_PBRMaterialTextures.emissive = Ref<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path + "/" + name + "/emissive" + extension, params));
            */
        }

        void RZPBRMaterial::loadShader(const std::string& path)
        {

        }

        void RZPBRMaterial::Bind()
        {
        }

    }    // namespace Graphics
}    // namespace Razix