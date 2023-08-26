#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Dear ImGui shader for drawing the UI elements
 */
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable

//------------------------------------------------------------------------------
// Vertex Input
layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;
//------------------------------------------------------------------------------
// Uniforms and Push Constants
layout (push_constant) uniform PushConstants {
	vec2 scale;
	vec2 translate;
} pushConstants;
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
layout(location = 0) out VSOutput
{
     vec2 fragUV;
     vec4 fragColor;
}vs_out;

out gl_PerVertex
{
    vec4 gl_Position;
};
//------------------------------------------------------------------------------
void main()
{
    // Final position of the vertices
    gl_Position = vec4(inPos * pushConstants.scale + pushConstants.translate, 0.0, 1.0);

    //float CPa = float((inColor & 0xff000000) >> 24) / 255.0;
    //float CPb = float((inColor & 0x00ff0000) >> 16) / 255.0;
    //float CPg = float((inColor & 0x0000ff00) >> 8) / 255.0;
    //float CPr = float((inColor & 0x000000ff)) / 255.0;

    // Out from vertex shader
	vs_out.fragUV = inUV;
    vs_out.fragColor    = inColor;
}
//------------------------------------------------------------------------------
