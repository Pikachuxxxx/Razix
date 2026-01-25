#ifndef _RZ_MATERIAL_ASSET_H_
#define _RZ_MATERIAL_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/RZHandle.h"

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
    RAZIX_REFLECT_STRING(AlbedoPath)
    RAZIX_REFLECT_STRING(NormalPath)
    RAZIX_REFLECT_STRING(MetallicPath)
    RAZIX_REFLECT_STRING(RoughnessPath)
    RAZIX_REFLECT_STRING(AmbientOcclusionPath)
    RAZIX_REFLECT_STRING(EmissivePath)
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
        rz_handle               AlbedoTextureHandle;
        rz_handle               NormalTextureHandle;
        rz_handle               MetallicTextureHandle;
        rz_handle               RoughnessTextureHandle;
        rz_handle               AmbientOcclusionTextureHandle;
        rz_handle               EmissiveTextureHandle;
        float3                  EmissiveColor;
        u8                      _pad0[4];
        RZMaterialsTexturePaths TexturePaths;
    };

    RAZIX_REFLECT_TYPE_START(RZMaterialAsset)
    RAZIX_REFLECT_PRIMITIVE(Albedo)
    RAZIX_REFLECT_PRIMITIVE(Metallic)
    RAZIX_REFLECT_PRIMITIVE(Roughness)
    RAZIX_REFLECT_PRIMITIVE(AmbientOcclusion)
    RAZIX_REFLECT_PRIMITIVE(NormalStrength)
    RAZIX_REFLECT_PRIMITIVE(EmissiveIntensity)
    RAZIX_REFLECT_PRIMITIVE(EmissiveColor)
    RAZIX_REFLECT_OBJECT(TexturePaths)
    RAZIX_REFLECT_TYPE_END(RZMaterialAsset)

};    // namespace Razix

#endif    // _RZ_MATERIAL_ASSET_H_
