#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Dear ImGui fragment shader for drawing the UI elements, binds a FontAtlas for font rendering or Sample for ImGui::Image()
 */
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable

//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec2 fragUV;
    vec4 fragColor;
}fs_in;

//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
layout(set = 0, binding = 0) uniform sampler2D fontSampler;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------

void main()
{
    vec4 color = fs_in.fragColor * texture(fontSampler, fs_in.fragUV);
    outFragColor = color;
}
//------------------------------------------------------------------------------
