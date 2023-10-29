#ifndef _MATERIAL_GLSL_
#define _MATERIAL_GLSL_
//----------------------------------------------------------------------------
//#include <Common/ShaderInclude.Builtin.BindlessResources.glsl>
//----------------------------------------------------------------------------
// Note:- When using a combined MetallicRoughnessAO map 
// .r = empty .g = Roughness .b  = Metalness .a = AO
//----------------------------------------------------------------------------
// Defined and Constants
const uint WORLFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED = 0;    // In the order of BGR components! AO = r, Roughness = g, Metal = b
const uint WORLFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE = 1;
const uint WORLFLOW_PBR_SPECULAR_GLOSS_COMBINED = 2;
const uint WORLFLOW_PBR_SPECULAR_GLOSS_SEPARATE = 3;
const uint WORKFLOW_UNLIT = 4;
const uint WORLFLOW_LIT_PHONG = 5;
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
    vec2 uvScale;
    bool visible;            
    uint workflow;

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
layout(set = 1, binding = 3) uniform sampler2D metallicMap; // Also can be used as combined metallic roughness AO map 
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
    uv *= Material.uvScale;
    return vec3(texture(albedoMap, uv));
    //if(Material.isUsingAlbedoMap)
    //    return vec3(texture(albedoMap, uv));
    //else 
    //    return Material.baseColor * Material.emissiveIntensity;
}
//----------------------------------------------------------------------------
vec3 Mat_getNormalMapNormals(vec2 uv, vec3 worldPos, vec3 N)
{
    uv *= Material.uvScale;
    if(Material.isUsingNormalMap) {
        vec3 tangentNormal = texture(normalMap, uv).xyz * 2.0 - 1.0;

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
    uv *= Material.uvScale;
    if (Material.isUsingMetallicMap && Material.workflow == WORLFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE)
        return vec3(texture(metallicMap, uv)).r;
    else if(Material.isUsingMetallicMap && Material.workflow == WORLFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED)
        return vec3(texture(metallicMap, uv)).b;
    else 
        return Material.metallic;
}
//----------------------------------------------------------------------------
float Mat_getRoughnessColor(vec2 uv)
{
    uv *= Material.uvScale;
    if(Material.isUsingRoughnessMap && Material.workflow == WORLFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE)
        return vec3(texture(roughnessMap, uv)).r;
    else if (Material.isUsingMetallicMap && Material.workflow == WORLFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED)
        return vec3(texture(metallicMap, uv)).g;
    else 
        return Material.roughness;
}
//----------------------------------------------------------------------------
vec3 getSpecularColor(vec2 uv)
{
    uv *= Material.uvScale;
    if(Material.isUsingSpecular)
        return vec3(texture(specularMap, uv));
    else 
        return vec3(1.0f);
}
//----------------------------------------------------------------------------
float Mat_getAOColor(vec2 uv)
{
    uv *= Material.uvScale;
    if(Material.isUsingAOMap && Material.workflow == WORLFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE)
        return vec3(texture(aoMap, uv)).r;
    else if (Material.isUsingMetallicMap && Material.workflow == WORLFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED)
        return vec3(texture(metallicMap, uv)).r;
    else 
        return 1.0f;
}
//----------------------------------------------------------------------------
float Mat_getOpacity(vec2 uv)
{
    uv *= Material.uvScale;
    if(Material.isUsingAlbedoMap)
        return texture(albedoMap, uv).a;
    else 
        return Material.opacity;
}
//----------------------------------------------------------------------------
#endif