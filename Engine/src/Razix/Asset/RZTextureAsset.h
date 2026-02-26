#ifndef _RZ_TEXTURE_ASSET_H_
#define _RZ_TEXTURE_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {

    struct RAZIX_ALIGN_TO(RAZIX_16B_ALIGN) RZTextureAsset
    {
        RAZIX_ASSET

        RZString            TexturePath;
        rz_handle           TextureHandle;
        rz_gfx_texture_desc Desc;
    };

    RAZIX_REFLECT_TYPE_START(rz_gfx_texture_desc)
    RAZIX_REFLECT_PRIMITIVE(width)
    RAZIX_REFLECT_PRIMITIVE(height)
    RAZIX_REFLECT_PRIMITIVE(depth)
    RAZIX_REFLECT_PRIMITIVE(mipLevels)
    RAZIX_REFLECT_PRIMITIVE(format)
    RAZIX_REFLECT_PRIMITIVE(textureType)
    RAZIX_REFLECT_PRIMITIVE(resourceHints)
    RAZIX_REFLECT_TYPE_END(rz_gfx_texture_desc)

    RAZIX_REFLECT_TYPE_START(RZTextureAsset)
    RAZIX_REFLECT_ASSET_HEADER
    RAZIX_REFLECT_STRING(TexturePath)
    RAZIX_REFLECT_OBJECT(Desc)
    RAZIX_REFLECT_TYPE_END(RZTextureAsset)

};    // namespace Razix

#endif    // _RZ_TEXTURE_ASSET_H_
