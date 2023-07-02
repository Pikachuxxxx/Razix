#ifndef _MATERIAL_GLSL_
#define _MATERIAL_GLSL_
//----------------------------------------------------------------------------
// Note:- When using a combined MetallicRoughnessAO map 
// .r = empty .g = Roughness .b  = Metalness .a = AO
//----------------------------------------------------------------------------
// Material Data and maps
layout(set = 1, binding = 0) uniform Material
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
} material;
//----------------------------------------------------------------------------
// Material Textures
layout(set = 1, binding = 1) uniform sampler2D albedoMap;
layout(set = 1, binding = 2) uniform sampler2D normalMap;
layout(set = 1, binding = 3) uniform sampler2D metallicMap;
layout(set = 1, binding = 4) uniform sampler2D roughnessMap;
layout(set = 1, binding = 5) uniform sampler2D specularMap;
layout(set = 1, binding = 6) uniform sampler2D emissiveMap;
layout(set = 1, binding = 7) uniform sampler2D aoMap;
//----------------------------------------------------------------------------
// Helper Functions
vec3 Mat_getAlbedoColor(vec2 uv)
{
    if(material.isUsingAlbedoMap)
        return vec3(texture(albedoMap, uv));
    else 
        return material.baseColor * material.emissiveIntensity;
}
//----------------------------------------------------------------------------
vec3 Mat_getNormalMapNormals(vec2 uv)
{
    if(material.isUsingNormalMap)
        return vec3(texture(normalMap, uv));
    else 
        return vec3(0.0f);
}
//----------------------------------------------------------------------------
float Mat_getMetallicColor(vec2 uv)
{
    if(material.isUsingMetallicMap)
        return vec3(texture(metallicMap, uv)).r;
    else 
        return material.metallic;
}
//----------------------------------------------------------------------------
float Mat_getRoughnessColor(vec2 uv)
{
    if(material.isUsingRoughnessMap)
        return vec3(texture(roughnessMap, uv)).r;
    else 
        return material.roughness;
}
//----------------------------------------------------------------------------
vec3 getSpecularColor(vec2 uv)
{
    if(material.isUsingSpecular)
        return vec3(texture(specularMap, uv));
    else 
        return vec3(1.0f);
}
//----------------------------------------------------------------------------
float Mat_getAOColor(vec2 uv)
{
    if(material.isUsingAOMap)
        return vec3(texture(aoMap, uv)).r;
    else 
        return 1.0f;
}
//----------------------------------------------------------------------------
float Mat_getOpacity(vec2 uv)
{
    if(material.isUsingAlbedoMap)
        return texture(albedoMap, uv).a;
    else 
        return material.opacity;
}
//----------------------------------------------------------------------------
#endif