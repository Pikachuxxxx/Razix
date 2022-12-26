/*
 * Razix Engine Shader File
 * Used to Render a Reflective Shadow Map
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable

//------------------------------------------------------------------------------
// Include Files
#include <Material/Material.glsl>
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec3 fragPos;
    vec4 fragColor;
    vec2 fragTexCoord;
    vec3 fragNormal;
    vec3 fragTangent;
}vs_in;
//------------------------------------------------------------------------------
// RTs
layout(location = 0) out vec4 worldPos;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 flux;
//------------------------------------------------------------------------------

void main()
{
    worldPos = vec4(vs_in.fragPos, 1.0f);

    if(material.isUsingNormalMap)
        normal = texture(normalMap, vs_in.fragTexCoord);
    else 
        normal = vec4(normalize(vs_in.fragNormal), 1.0f);
        
    vec3 color;
    if(material.isUsingAlbedoMap)
        color = material.baseColor;
    else 
        color =  texture(albedoMap, vs_in.fragTexCoord).rgb;

    flux = vec4((color) + material.emissiveColor, 1.0f);
}