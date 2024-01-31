/*
 * Razix Engine GLSL Vertex Shader File
 * Pixel Shader to render the G-Buffer render targets
 */
#version 450 core
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
//#define ENABLE_BINDLESS 1
// Includes
#include <Material/ShaderInclude.Builtin.Material.glsl>
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec3 fragPos;
    vec4 fragColor;
    vec2 fragUV;
    vec3 fragNormal;
    vec3 fragTangent;
    vec3 viewPos;
}fs_in;
//------------------------------------------------------------------------------ 
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 GBuffer0;    // .rgb = Normal   .a = Metallic
layout(location = 1) out vec4 GBuffer1;    // .rgb = Albedo   .a = Roughness
layout(location = 2) out vec4 GBuffer2;    // .rgb = Position .a = AO
//------------------------------------------------------------------------------
void main()
{
    vec3 albedo = Mat_getAlbedoColor(fs_in.fragUV);
    float metallic = Mat_getMetallicColor(fs_in.fragUV);
    float roughness = Mat_getRoughnessColor(fs_in.fragUV);
    float ao = Mat_getAOColor(fs_in.fragUV);

    // Write the Normals to the GBuffer0  
    vec3 N_Surface = normalize(fs_in.fragNormal);
    vec3 N = normalize(Mat_getNormalMapNormals(fs_in.fragUV, fs_in.fragPos, N_Surface));
    GBuffer0 = vec4(N, metallic);

    GBuffer1 = vec4(albedo.rgb, roughness);
    GBuffer2 = vec4(fs_in.fragPos.rgb, ao);
}
//------------------------------------------------------------------------------