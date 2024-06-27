// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZTexture.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLTexture.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKTexture.h"
#endif

#include "Razix/Graphics/RHI/API/RZShader.h"

#include "Razix/Graphics/Resources/IRZResource.h"

namespace Razix {
    namespace Graphics {

        //-----------------------------------------------------------------------------------
        // Texture
        //-----------------------------------------------------------------------------------

        class DX12Texture
        {};
        GET_INSTANCE_SIZE_IMPL(Texture)

        void RZTexture::Create(void* where, const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // If we are loading from a file
            if (!desc.filePath.empty()) {
                RZTexture::CreateFromFile(where, desc, desc.filePath RZ_DEBUG_E_ARG_NAME);
                return;
            }

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Graphics::RenderAPI::OPENGL: new (where) OpenGLTexture(desc); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Graphics::RenderAPI::VULKAN: new (where) VKTexture(desc RZ_DEBUG_E_ARG_NAME); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Graphics::RenderAPI::D3D12:
#endif
                default: break;
            }
        }

        void RZTexture::CreateFromFile(void* where, const RZTextureDesc& desc, const std::string& filePath RZ_DEBUG_NAME_TAG_E_ARG)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Graphics::RenderAPI::OPENGL: new (where) OpenGLTexture(desc, filePath); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Graphics::RenderAPI::VULKAN: new (where) VKTexture(desc, filePath RZ_DEBUG_E_ARG_NAME); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Graphics::RenderAPI::D3D12:
#endif
                default: break;
            }
        }

        u32 RZTexture::calculateMipMapCount(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            //tex: The texture mip levels are $numLevels = 1 + floor(log2(max(w, h, d)))$
            u32 levels = 1;
            while ((width | height) >> levels)
                levels++;

            return levels;
        }

        TextureFormat RZTexture::bitsToTextureFormat(u32 bits)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch ((bits * 8)) {
                case 8:
                    return TextureFormat::R8;
                case 16:
                    return TextureFormat::RG8;
                case 24:
                    return TextureFormat::RGB8;
                case 32:
                    return TextureFormat::RGBA8;
                case 48:
                    return TextureFormat::RGB16;
                case 64:
                    return TextureFormat::RGBA16;
                default:
                    RAZIX_CORE_ASSERT(false, "[Texture] Unsupported image bit-depth! ({0})", bits);
                    return TextureFormat::RGB8;
            }
        }

        void RZTexture::generateDescriptorSet()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZDescriptor descriptor{};
            descriptor.name                         = m_Desc.name;
            descriptor.bindingInfo.location.binding = 0;
            descriptor.bindingInfo.count            = 1;
            descriptor.bindingInfo.stage            = ShaderStage::Pixel;
            descriptor.bindingInfo.type             = DescriptorType::ImageSamplerCombined;
            descriptor.texture                      = this->m_Handle;

            if (!m_DescriptorSet)
                m_DescriptorSet = Graphics::RZDescriptorSet::Create({descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG(descriptor.name));
        }
    }    // namespace Graphics
}    // namespace Razix