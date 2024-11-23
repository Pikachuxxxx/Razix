/*
 * Razix Engine Shader File
 * Mesh fragment shader used to render meshes with a plain unlit albedo texture
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable

 //------------------------------------------------------------------------------
 // Vertex Input
layout(location = 0) in VSOutput
{
    vec3 fragPos;
    vec4 fragColor;
    vec2 fragUV;
    vec3 fragNormal;
    vec3 fragTangent;
}fs_in;

 //------------------------------------------------------------------------------
 // Fragment Shader Stage Uniforms
 // Albedo map with which the mesh will be rendered with
layout(set = 1, binding = 0) uniform sampler2D albedoMap;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------

void main()
{
    outFragColor = texture(albedoMap, fs_in.fragUV);
}
//------------------------------------------------------------------------------
