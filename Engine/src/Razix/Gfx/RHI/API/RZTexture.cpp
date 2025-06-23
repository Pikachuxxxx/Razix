// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZTexture.h"

#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
#include "Razix/Gfx/RHI/API/RZShader.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKTexture.h"
#endif

namespace Razix {
    namespace Gfx {

        //-----------------------------------------------------------------------------------
        // Texture
        //-----------------------------------------------------------------------------------

        RZTextureHandle RZTexture::s_DefaultTexture = {};

        void RZTexture::InitDefaultTexture()
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

        void RZTexture::ReleaseDefaultTexture()
        {
            RZResourceManager::Get().destroyTexture(s_DefaultTexture);
        }

        class DX12Texture
        {};
        GET_INSTANCE_SIZE_IMPL(Texture)

        void RZTexture::Create(void* where, const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Gfx::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Gfx::RenderAPI::VULKAN: new (where) VKTexture(desc RZ_DEBUG_E_ARG_NAME); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Gfx::RenderAPI::D3D12: RAZIX_DEBUG_BREAK(); break;
#endif
                default: break;
            }
        }

        u32 RZTexture::CalculateMipMapCount(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return static_cast<u32>(std::floor(std::log2(std::max(width, height)))) + 1;
        }

        TextureFormat RZTexture::BitsToTextureFormat(u32 bits)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch ((bits)) {
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
    }    // namespace Gfx
}    // namespace Razix