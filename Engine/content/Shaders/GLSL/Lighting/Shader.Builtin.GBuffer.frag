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
    // Since the current GLTF models have a MetallicgRoughNesAO maps we hard code this shit ( GLTF texutes .r = empty (mostly) .g = roughness .b = metallic .a = AO)
#ifdef FUSED_MRAO
    vec4 MetallicRoughnessAO = texture(metallicMap, fs_in.fragUV);
#endif

    // Write the Normals to the GBuffer0  
    GBuffer0 = vec4(normalize(fs_in.fragNormal), Mat_getMetallicColor(fs_in.fragUV));
    GBuffer1 = vec4(Mat_getAlbedoColor(fs_in.fragUV).rgb, Mat_getRoughnessColor(fs_in.fragUV));
    GBuffer2 = vec4(fs_in.fragPos.rgb, Mat_getAOColor(fs_in.fragUV));
}
//------------------------------------------------------------------------------