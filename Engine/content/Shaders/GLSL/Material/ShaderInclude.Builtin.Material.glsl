#ifndef _MATERIAL_GLSL_
#define _MATERIAL_GLSL_
//----------------------------------------------------------------------------
#include <Common/ShaderInclude.Builtin.BindlessResources.glsl>
//----------------------------------------------------------------------------
// Note:- When using a combined MetallicRoughnessAO map 
// .r = empty .g = Roughness .b  = Metalness .a = AO
//----------------------------------------------------------------------------
// Material Data and maps
//layout(set = 2, binding = 0) uniform
struct Material
{
    vec3  baseColor;
    vec3  normal;
    float emissiveIntensity;
    float metallic;     
    float roughness;    
    float specular;     
    float opacity;            
    float ambientOcclusion;   
    bool  visible;            

    bool isUsingAlbedoMap;
    bool isUsingNormalMap;
    bool isUsingMetallicMap;
    bool isUsingRoughnessMap;
    bool isUsingSpecular;
    bool isUsingEmissiveMap;
    bool isUsingAOMap;    

    uint AlbedoMapIdx;
    uint NormalMapIdx;
    uint MetallicMapIdx;
    uint RoughnessMapIdx;
    uint SpecularMapIdx;
    uint EmissiveMapIdx;
    uint AOMapIdx;
} material;
//----------------------------------------------------------------------------
// Material Textures
#if !ENABLE_BINDLESS
layout(set = 2, binding = 1) uniform sampler2D albedoMap;
layout(set = 2, binding = 2) uniform sampler2D normalMap;
layout(set = 2, binding = 3) uniform sampler2D metallicMap;
layout(set = 2, binding = 4) uniform sampler2D roughnessMap;
layout(set = 2, binding = 5) uniform sampler2D specularMap;
layout(set = 2, binding = 6) uniform sampler2D emissiveMap;
layout(set = 2, binding = 7) uniform sampler2D aoMap;
#endif
//------------------------------------------------------------------------------
// Material Data [Bindless]
layout (std140, set = SET_IDX_BINDLESS_RESOURCES_START, binding = GLOBAL_BINDLESS_MATERIAL_STORAGE_BUFFERS_BINDING_IDX ) readonly buffer MaterialBuffer
{
    Material materials[];
}matBuffer;
//----------------------------------------------------------------------------
// Helper Functions
vec3 Mat_getAlbedoColor(vec2 uv)
{
    if(material.isUsingAlbedoMap)
#if ENABLE_BINDLESS
        return texture(global_textures_2d[nonuniformEXT(material.AlbedoMapIdx)], uv).rgb * material.emissiveIntensity;
#else
        return vec3(texture(albedoMap, uv)) * material.emissiveIntensity;
#endif
    else 
        return material.baseColor * material.emissiveIntensity;
}
//----------------------------------------------------------------------------
vec3 Mat_getNormalMapNormals(vec2 uv, vec3 worldPos, vec3 N)
{
    if(material.isUsingNormalMap) {
#if ENABLE_BINDLESS
        vec3 tangentNormal = texture(global_textures_2d[nonuniformEXT(material.NormalMapIdx)], uv).rgb;
#else
        vec3 tangentNormal = texture(normalMap, uv).xyz * 2.0 - 1.0;
#endif

        vec3 Q1  = dFdx(worldPos);
        vec3 Q2  = dFdy(worldPos);
        vec2 st1 = dFdx(uv);
        vec2 st2 = dFdy(uv);

        vec3 N   = normalize(N);
        vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
        vec3 B  = -normalize(cross(N, T));
        mat3 TBN = mat3(T, B, N);

        return normalize(TBN * tangentNormal);
    }
    else 
        return N;
}
//----------------------------------------------------------------------------
float Mat_getMetallicColor(vec2 uv)
{
    if(material.isUsingMetallicMap)
#if ENABLE_BINDLESS
        return texture(global_textures_2d[nonuniformEXT(material.MetallicMapIdx)], uv).r;
#else
        return vec3(texture(metallicMap, uv)).r;
#endif
    else 
        return material.metallic;
}
//----------------------------------------------------------------------------
float Mat_getRoughnessColor(vec2 uv)
{
    if(material.isUsingRoughnessMap)
#if ENABLE_BINDLESS
        return texture(global_textures_2d[nonuniformEXT(material.RoughnessMapIdx)], uv).r;
#else
        return vec3(texture(roughnessMap, uv)).r;
#endif
    else 
        return material.roughness;
}
//----------------------------------------------------------------------------
vec3 getSpecularColor(vec2 uv)
{
    if(material.isUsingSpecular)
#if ENABLE_BINDLESS
        return texture(global_textures_2d[nonuniformEXT(material.SpecularMapIdx)], uv).rgb;
#else
        return vec3(texture(specularMap, uv));
#endif
    else 
        return vec3(1.0f);
}
//----------------------------------------------------------------------------
float Mat_getAOColor(vec2 uv)
{
    if(material.isUsingAOMap)
#if ENABLE_BINDLESS
        return texture(global_textures_2d[nonuniformEXT(material.AOMapIdx)], uv).r;
#else
        return vec3(texture(aoMap, uv)).r;
#endif
    else 
        return 1.0f;
}
//----------------------------------------------------------------------------
float Mat_getOpacity(vec2 uv)
{
    if(material.isUsingAlbedoMap)
#if ENABLE_BINDLESS
        return texture(global_textures_2d[nonuniformEXT(material.AlbedoMapIdx)], uv).a;
#else
        return texture(albedoMap, uv).a;
#endif
    else 
        return material.opacity;
}
//----------------------------------------------------------------------------
#endif