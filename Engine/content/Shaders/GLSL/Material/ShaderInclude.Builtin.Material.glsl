#ifndef _MATERIAL_GLSL_
#define _MATERIAL_GLSL_
//----------------------------------------------------------------------------
//#include <Common/ShaderInclude.Builtin.BindlessResources.glsl>
//----------------------------------------------------------------------------
// Note:- When using a combined MetallicRoughnessAO map 
// .r = empty .g = Roughness .b  = Metalness .a = AO
//----------------------------------------------------------------------------
// Material Data and maps
layout(set = 1, binding = 0) uniform MaterialData
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
} Material;
//----------------------------------------------------------------------------
// Material Textures
layout(set = 1, binding = 1) uniform sampler2D albedoMap;
layout(set = 1, binding = 2) uniform sampler2D normalMap;
layout(set = 1, binding = 3) uniform sampler2D metallicMap;
layout(set = 1, binding = 4) uniform sampler2D roughnessMap;
layout(set = 1, binding = 5) uniform sampler2D specularMap;
layout(set = 1, binding = 6) uniform sampler2D emissiveMap;
layout(set = 1, binding = 7) uniform sampler2D aoMap;
//------------------------------------------------------------------------------
// Material Data [Bindless]
//layout (std140, set = SET_IDX_BINDLESS_RESOURCES_START, binding = GLOBAL_BINDLESS_MATERIAL_STORAGE_BUFFERS_BINDING_IDX ) readonly buffer MaterialBuffer
//{
//    Material Materials[];
//}matBuffer;
//----------------------------------------------------------------------------
// Helper Functions
vec3 Mat_getAlbedoColor(vec2 uv)
{
    if(Material.isUsingAlbedoMap)
//#if ENABLE_BINDLESS
//        return texture(global_textures_2d[nonuniformEXT(Material.AlbedoMapIdx)], uv).rgb * Material.emissiveIntensity;
//#else
        return vec3(texture(albedoMap, uv)) * Material.emissiveIntensity;
//#endif
    else 
        return Material.baseColor * Material.emissiveIntensity;
}
//----------------------------------------------------------------------------
vec3 Mat_getNormalMapNormals(vec2 uv, vec3 worldPos, vec3 N)
{
    if(Material.isUsingNormalMap) {
//#if ENABLE_BINDLESS
//        vec3 tangentNormal = texture(global_textures_2d[nonuniformEXT(Material.NormalMapIdx)], uv).rgb;
//#else
        vec3 tangentNormal = texture(normalMap, uv).xyz * 2.0 - 1.0;
//#endif

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
    if(Material.isUsingMetallicMap)
//#if ENABLE_BINDLESS
//        return texture(global_textures_2d[nonuniformEXT(Material.MetallicMapIdx)], uv).r;
//#else
        return vec3(texture(metallicMap, uv)).r;
//#endif
    else 
        return Material.metallic;
}
//----------------------------------------------------------------------------
float Mat_getRoughnessColor(vec2 uv)
{
    if(Material.isUsingRoughnessMap)
//#if ENABLE_BINDLESS
//        return texture(global_textures_2d[nonuniformEXT(Material.RoughnessMapIdx)], uv).r;
//#else
        return vec3(texture(roughnessMap, uv)).r;
//#endif
    else 
        return Material.roughness;
}
//----------------------------------------------------------------------------
vec3 getSpecularColor(vec2 uv)
{
    if(Material.isUsingSpecular)
//#if ENABLE_BINDLESS
//        return texture(global_textures_2d[nonuniformEXT(Material.SpecularMapIdx)], uv).rgb;
//#else
        return vec3(texture(specularMap, uv));
//#endif
    else 
        return vec3(1.0f);
}
//----------------------------------------------------------------------------
float Mat_getAOColor(vec2 uv)
{
    if(Material.isUsingAOMap)
//#if ENABLE_BINDLESS
//        return texture(global_textures_2d[nonuniformEXT(Material.AOMapIdx)], uv).r;
//#else
        return vec3(texture(aoMap, uv)).r;
//#endif
    else 
        return 1.0f;
}
//----------------------------------------------------------------------------
float Mat_getOpacity(vec2 uv)
{
    if(Material.isUsingAlbedoMap)
//#if ENABLE_BINDLESS
//        return texture(global_textures_2d[nonuniformEXT(Material.AlbedoMapIdx)], uv).a;
//#else
        return texture(albedoMap, uv).a;
//#endif
    else 
        return Material.opacity;
}
//----------------------------------------------------------------------------
#endif