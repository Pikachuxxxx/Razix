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
// Includes
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
    vec3 viewPos;
}fs_in;
//------------------------------------------------------------------------------ 
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 GBuffer0; // .rgb = Normal .a = Position.x
layout(location = 1) out vec4 GBuffer1; // .rgb = Albedo .a = Position.y
layout(location = 2) out vec4 GBuffer2; // .rgb = Position .a = Position.z
layout(location = 3) out vec4 GBuffer3; // .r = Metallic .g = roughness .b = AO .a = alpha/opacity
//------------------------------------------------------------------------------
void main()
{
    // Write the Normals to the GBuffer0  
    GBuffer0 = vec4(normalize(fs_in.fragNormal), 1.0f);

    GBuffer1 = vec4(texture(albedoMap, fs_in.fragTexCoord).rgb, 1.0f);
    GBuffer2 = vec4(vec3(fs_in.fragPos), 1.0f);

    // Since the current GLTTF modedls have a MetallicgRoughNesAO maps we hard code this shit ( GLTF texutes .r = empty (mostly) .g = roughness .b = metallic .a = AO)
    vec4 MetallicRoughnessAO = texture(metallicMap, fs_in.fragTexCoord);

    // GBuffer3 :: .r = Metallic .g = roughness .b = AO .a = specular
    GBuffer3 = vec4( MetallicRoughnessAO.b, MetallicRoughnessAO.g, MetallicRoughnessAO.a, material.opacity);
}
//------------------------------------------------------------------------------

