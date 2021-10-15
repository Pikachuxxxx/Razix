#include "rzxpch.h"
#include "Texture.h"


namespace Razix {
    namespace Graphics {

        //-----------------------------------------------------------------------------------
        // Texture
        //-----------------------------------------------------------------------------------

        uint8_t Texture::GetStrideFromFormat(TextureFormat format) {
            switch (format) {
                case TextureFormat::RGB:
                    return 3;
                case TextureFormat::RGBA:
                    return 4;
                case TextureFormat::R8:
                    return 1;
                case TextureFormat::RG8:
                    return 2;
                case TextureFormat::RGB8:
                    return 3;
                case TextureFormat::RGBA8:
                    return 4;
                default:
                    return 0;
            }
        }

        TextureFormat Texture::BitsToTextureFormat(uint32_t bits) {
            switch (bits) {
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
                    break;
            }
        }

        uint32_t Texture::CalculateMipMapCount(uint32_t width, uint32_t height) {
            uint32_t levels = 1;
            while ((width | height) >> levels)
                levels++;

            return levels;
        }

        //-----------------------------------------------------------------------------------
        // Texture2D
        //-----------------------------------------------------------------------------------


        Texture2D* Texture2D::Create() {

        }

        Texture2D* Texture2D::Create(uint32_t width, uint32_t height, void* data, TextureLoadOptions loadOptions /*= TextureLoadOptions()*/, TextureParameters parameters /*= TextureParameters()*/) {

        }

        Texture2D* Texture2D::CreateFromFile(std::string name, std::string pathName, TextureLoadOptions loadOptions /*= TextureLoadOptions()*/, TextureParameters parameters /*= TextureParameters()*/) {

        }

    }
}