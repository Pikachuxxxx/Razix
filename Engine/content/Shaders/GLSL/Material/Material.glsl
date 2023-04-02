#ifndef _MATERIAL_GLSL_
#define _MATERIAL_GLSL_

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

layout(set = 1, binding = 1) uniform sampler2D albedoMap;
layout(set = 1, binding = 2) uniform sampler2D normalMap;
layout(set = 1, binding = 3) uniform sampler2D metallicMap;
layout(set = 1, binding = 4) uniform sampler2D roughnessMap;
layout(set = 1, binding = 5) uniform sampler2D specularMap;
layout(set = 1, binding = 6) uniform sampler2D emissiveMap;
layout(set = 1, binding = 7) uniform sampler2D aoMap;

vec3 getAlbedoColor(vec2 uv)
{
    if(material.isUsingAlbedoMap)
        return vec3(texture(albedoMap, uv)) * material.baseColor * material.emissiveIntensity;
    else 
        return material.baseColor * material.emissiveIntensity;
}

vec3 getSpecularColor(vec2 uv)
{
    if(material.isUsingSpecular)
        return vec3(texture(specularMap, uv));
    else 
        return vec3(1.0f);
}

float getOpacity(vec2 uv)
{
    if(material.isUsingAlbedoMap)
        return texture(albedoMap, uv).a;
    else 
        return material.opacity;
}

vec3 getNormals(vec2 uv, vec3 normals)
{
    //if(material.isUsingNormalMap)
    //    return vec3(texture(normalMap, uv));
    //else 
    return normals;
}

#endif