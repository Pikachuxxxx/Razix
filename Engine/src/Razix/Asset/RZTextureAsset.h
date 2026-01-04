#ifndef _RZ_TEXTURE_ASSET_H_
#define _RZ_TEXTURE_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {

    struct RAZIX_ALIGN_TO(RAZIX_16B_ALIGN) RZTextureAsset
    {
        RZString            TexturePath;
        rz_handle           TextureHandle;
        rz_gfx_texture_desc Desc;
    };

    RAZIX_REFLECT_TYPE_START(RZTextureAsset)
    RAZIX_REFLECT_MEMBER(TexturePath)
    RAZIX_REFLECT_MEMBER(Desc.width)
    RAZIX_REFLECT_MEMBER(Desc.height)
    RAZIX_REFLECT_MEMBER(Desc.depth)
    RAZIX_REFLECT_MEMBER(Desc.mipLevels)
    RAZIX_REFLECT_MEMBER(Desc.format)
    RAZIX_REFLECT_MEMBER(Desc.textureType)
    RAZIX_REFLECT_MEMBER(Desc.resourceHints)
    RAZIX_REFLECT_TYPE_END(RZTextureAsset)

};    // namespace Razix

#endif    // _RZ_TEXTURE_ASSET_H_
