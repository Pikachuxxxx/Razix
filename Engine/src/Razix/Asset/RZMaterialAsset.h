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
    RAZIX_REFLECT_MEMBER(AlbedoPath)
    RAZIX_REFLECT_MEMBER(NormalPath)
    RAZIX_REFLECT_MEMBER(MetallicPath)
    RAZIX_REFLECT_MEMBER(RoughnessPath)
    RAZIX_REFLECT_MEMBER(AmbientOcclusionPath)
    RAZIX_REFLECT_MEMBER(EmissivePath)
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
    RAZIX_REFLECT_MEMBER(Albedo)
    RAZIX_REFLECT_MEMBER(Metallic)
    RAZIX_REFLECT_MEMBER(Roughness)
    RAZIX_REFLECT_MEMBER(AmbientOcclusion)
    RAZIX_REFLECT_MEMBER(AlbedoTextureHandle)
    RAZIX_REFLECT_MEMBER(NormalTextureHandle)
    RAZIX_REFLECT_MEMBER(MetallicTextureHandle)
    RAZIX_REFLECT_MEMBER(RoughnessTextureHandle)
    RAZIX_REFLECT_MEMBER(AmbientOcclusionTextureHandle)
    RAZIX_REFLECT_MEMBER(EmissiveTextureHandle)
    RAZIX_REFLECT_MEMBER(NormalStrength)
    RAZIX_REFLECT_MEMBER(EmissiveIntensity)
    RAZIX_REFLECT_MEMBER(EmissiveColor)
    RAZIX_REFLECT_MEMBER(TexturePaths)
    RAZIX_REFLECT_TYPE_END(RZMaterialAsset)

};    // namespace Razix

#endif    // _RZ_MATERIAL_ASSET_H_
