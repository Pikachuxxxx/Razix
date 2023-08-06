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

#define GET_INSTANCE_SIZE_IMPL(TYPE)                                                        \
    u32 TYPE::GetInstanceSize()                                                             \
    {                                                                                       \
        switch (Graphics::RZGraphicsContext::GetRenderAPI()) {                              \
            case Razix::Graphics::RenderAPI::OPENGL: return sizeof(OpenGLTexture2D); break; \
            case Razix::Graphics::RenderAPI::VULKAN: return sizeof(VKTexture2D); break;     \
            case Razix::Graphics::RenderAPI::D3D11:                                         \
            case Razix::Graphics::RenderAPI::D3D12:                                         \
            case Razix::Graphics::RenderAPI::GXM:                                           \
            case Razix::Graphics::RenderAPI::GCM:                                           \
            default: return sizeof(RZTexture); break;                                       \
        }                                                                                   \
    }
        GET_INSTANCE_SIZE_IMPL(RZTexture)
        //GET_INSTANCE_SIZE_IMPL(RZTexture2D)
        //GET_INSTANCE_SIZE_IMPL(RZTexture3D)

        //-----------------------------------------------------------------------------------
        // Texture
        //-----------------------------------------------------------------------------------

        RZTextureHandle RZTexture::Create(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLTexture(desc); break;
                case Razix::Graphics::RenderAPI::VULKAN: return (new VKTexture2D(desc RZ_DEBUG_E_ARG_NAME))->getHandle(); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return RZTextureHandle(); break;
            }
            return RZTextureHandle();
        }

        RZTextureHandle RZTexture::CreateFromFile(const std::string& filePath, const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLTexture(desc, filePath); break;
                case Razix::Graphics::RenderAPI::VULKAN: return (new VKTexture2D(desc, filePath RZ_DEBUG_E_ARG_NAME))->getHandle(); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return RZTextureHandle(); break;
            }
            return RZTextureHandle();
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

        RZTextureProperties::Format RZTexture::bitsToTextureFormat(u32 bits)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch ((bits * 8)) {
                case 8:
                    return RZTextureProperties::Format::R8;
                case 16:
                    return RZTextureProperties::Format::RG8;
                case 24:
                    return RZTextureProperties::Format::RGB8;
                case 32:
                    return RZTextureProperties::Format::RGBA8;
                case 48:
                    return RZTextureProperties::Format::RGB16;
                case 64:
                    return RZTextureProperties::Format::RGBA16;
                default:
                    RAZIX_CORE_ASSERT(false, "[Texture] Unsupported image bit-depth! ({0})", bits);
                    return RZTextureProperties::Format::RGB8;
            }
        }

        void RZTexture::generateDescriptorSet()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZDescriptor descriptor{};
            descriptor.name                = m_Desc.name;
            descriptor.bindingInfo.binding = 0;
            descriptor.bindingInfo.count   = 1;
            descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
            descriptor.bindingInfo.type    = DescriptorType::IMAGE_SAMPLER;
            //descriptor.texture             = (RZTexture2D*) this;

            std::vector<RZDescriptor> descriptors = {descriptor};

            m_DescriptorSet = Graphics::RZDescriptorSet::Create(descriptors RZ_DEBUG_NAME_TAG_STR_E_ARG(descriptor.name));
        }

#if 0
        //-----------------------------------------------------------------------------------
        // Texture 2D
        //-----------------------------------------------------------------------------------

        RZTexture2D* RZTexture2D::Create(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLTexture2D(desc); break;
                //case Razix::Graphics::RenderAPI::VULKAN: return (new VKTexture2D(RZ_DEBUG_F_ARG_NAME desc))->getHandle(); break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKTexture2D(RZ_DEBUG_F_ARG_NAME desc); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
            return nullptr;
        }

        RZTexture2D* RZTexture2D::CreateArray(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLTexture2D(desc); break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKTexture2DArray(RZ_DEBUG_F_ARG_NAME desc); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
            return nullptr;
        }

        RZTexture2D* RZTexture2D::CreateFromFile(RZ_DEBUG_NAME_TAG_F_ARG const std::string& filePath, const RZTextureDesc& desc)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLTexture2D(filePath, desc); break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKTexture2D(RZ_DEBUG_F_ARG_NAME filePath, desc); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
            return nullptr;
        }

        //-----------------------------------------------------------------------------------
        // Texture 3D
        //-----------------------------------------------------------------------------------

        RZTexture3D* RZTexture3D::Create(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return nullptr; break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKTexture3D(RZ_DEBUG_F_ARG_NAME desc); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
            return nullptr;
        }

        //-----------------------------------------------------------------------------------
        // Cube Map Texture
        //-----------------------------------------------------------------------------------

        RZCubeMap* RZCubeMap::Create(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKCubeMap(RZ_DEBUG_F_ARG_NAME desc); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
            return nullptr;
        }

        //-----------------------------------------------------------------------------------
        // Depth Texture
        //-----------------------------------------------------------------------------------

        RZDepthTexture* RZDepthTexture::Create(const RZTextureDesc& desc)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return nullptr; break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKDepthTexture(desc); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
            return nullptr;
        }

        //-----------------------------------------------------------------------------------
        // Render Texture
        //-----------------------------------------------------------------------------------

        RZRenderTexture* RZRenderTexture::Create(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKRenderTexture(RZ_DEBUG_F_ARG_NAME desc); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
            return nullptr;
        }
#endif
    }    // namespace Graphics
}    // namespace Razix