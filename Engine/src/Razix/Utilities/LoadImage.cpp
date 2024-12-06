// clang-format off
#include "rzxpch.h"
// clang-format on
#include "LoadImage.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Razix {

    u8* Utilities::LoadImageData(const std::string& filePath, u32* width, u32* height, u32* bpp, bool flipY)
    {
        std::string physicalPath;
        if (!RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, physicalPath))
            return nullptr;

        int      texWidth = 0, texHeight = 0, texChannels = 0;
        stbi_uc* pixels = nullptr;

        stbi_set_flip_vertically_on_load(flipY);
        pixels = stbi_load(physicalPath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        RAZIX_CORE_ASSERT((pixels != nullptr), "Could not load image from : {0}", physicalPath);

        // TODO: support different texChannels

        if (width)
            *width = texWidth;
        if (height)
            *height = texHeight;
        if (bpp)
            *bpp = texChannels;

        const int32_t size   = texWidth * texHeight * 4;
        u8*           result = new u8[size];
        memcpy(result, pixels, size);

        stbi_image_free(pixels);
        return result;
    }

    RAZIX_API f32* Utilities::LoadImageDataFloat(const std::string& filePath, u32* width, u32* height, u32* bpp)
    {
        std::string physicalPath;
        if (!RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, physicalPath))
            return nullptr;

        int  texWidth = 0, texHeight = 0, texChannels = 0;
        f32* pixels = nullptr;

        stbi_set_flip_vertically_on_load(false);
        pixels = stbi_loadf(physicalPath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        RAZIX_CORE_ASSERT((pixels != nullptr), "Could not load image from : {0}", physicalPath);

        // TODO: support different texChannels

        if (width)
            *width = texWidth;
        if (height)
            *height = texHeight;
        if (bpp)
            *bpp = texChannels;

        const uint32_t size   = texWidth * texHeight * 4 * sizeof(float);    // 4 cause we used STBI_rgb_alpha and this will force the image to load in a 4-channel format
        f32*           result = new f32[size];
        memcpy(result, pixels, size);

        stbi_image_free(pixels);
        return result;
    }
}    // namespace Razix