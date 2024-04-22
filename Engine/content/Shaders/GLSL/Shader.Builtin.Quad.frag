/*
 * Razix Engine Shader File
 * Default Fragment Shader that can be used for rendering basic geometry with versampleColor colors and use a sampleColorture as well
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
// Bindless Textures
//#define ENABLE_BINDLESS 1
//#include <Common/ShaderInclude.Builtin.BindlessResources.glsl>
#include <FX/ShaderInclude.Builtin.Tonemapping.glsl>
//------------------------------------------------------------------------------
// VersampleColor Input
layout(location = 0) in VSOutput
{
    vec2 uv;
}fs_in;

//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
layout(set = 0, binding = 0) uniform sampler2D renderTarget;

//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------
void main()
{
    vec3 result = texture(renderTarget, fs_in.uv).rgb;
    outFragColor = vec4(result, 1.0f);
}
//------------------------------------------------------------------------------
