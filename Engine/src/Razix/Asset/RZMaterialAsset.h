#ifndef _RZ_MATERIAL_ASSET_H_
#define _RZ_MATERIAL_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Core/Containers/string.h"

namespace Razix {

    struct RZMaterialsTexturePaths
    {
        RZString AlbedoPath;
        RZString NormalPath;
        RZString MetallicPath;
        RZString RoughnessPath;
        RZString AmbientOcclusionPath;
        RZString EmissivePath;
    };

    RAZIX_REFLECT_TYPE_START(RZMaterialsTexturePaths)
    RAZIX_REFLECT_PRIMITIVE(AlbedoPath)
    RAZIX_REFLECT_PRIMITIVE(NormalPath)
    RAZIX_REFLECT_PRIMITIVE(MetallicPath)
    RAZIX_REFLECT_PRIMITIVE(RoughnessPath)
    RAZIX_REFLECT_PRIMITIVE(AmbientOcclusionPath)
    RAZIX_REFLECT_PRIMITIVE(EmissivePath)
    RAZIX_REFLECT_TYPE_END(RZMaterialsTexturePaths)

    // TODO: to be ratified later once RZMaterial system is done
    struct RAZIX_ALIGN_TO(RAZIX_16B_ALIGN) RZMaterialAsset
    {
        float4                  Albedo;
        f32                     Metallic;
        f32                     Roughness;
        f32                     AmbientOcclusion;
        f32                     NormalStrength;
        f32                     EmissiveIntensity;
        u32                     AlbedoTextureHandle;
        u32                     NormalTextureHandle;
        u32                     MetallicTextureHandle;
        u32                     RoughnessTextureHandle;
        u32                     AmbientOcclusionTextureHandle;
        u32                     EmissiveTextureHandle;
        float3                  EmissiveColor;
        u8                      _pad0[4];
        RZMaterialsTexturePaths TexturePaths;
    };

    RAZIX_REFLECT_TYPE_START(RZMaterialAsset)
    RAZIX_REFLECT_PRIMITIVE(Albedo)
    RAZIX_REFLECT_PRIMITIVE(Metallic)
    RAZIX_REFLECT_PRIMITIVE(Roughness)
    RAZIX_REFLECT_PRIMITIVE(AmbientOcclusion)
    RAZIX_REFLECT_PRIMITIVE(AlbedoTextureHandle)
    RAZIX_REFLECT_PRIMITIVE(NormalTextureHandle)
    RAZIX_REFLECT_PRIMITIVE(MetallicTextureHandle)
    RAZIX_REFLECT_PRIMITIVE(RoughnessTextureHandle)
    RAZIX_REFLECT_PRIMITIVE(AmbientOcclusionTextureHandle)
    RAZIX_REFLECT_PRIMITIVE(EmissiveTextureHandle)
    RAZIX_REFLECT_PRIMITIVE(NormalStrength)
    RAZIX_REFLECT_PRIMITIVE(EmissiveIntensity)
    RAZIX_REFLECT_PRIMITIVE(EmissiveColor)
    RAZIX_REFLECT_PRIMITIVE(TexturePaths)
    RAZIX_REFLECT_TYPE_END(RZMaterialAsset)

};    // namespace Razix

#endif    // _RZ_MATERIAL_ASSET_H_
