#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Sprite shader used to render colored sprtites aka 2D screen space quads in the scene
 */
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable

//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
//------------------------------------------------------------------------------
// Uniforms and Push Constants
layout (push_constant) uniform ModelPushConstantData{
    mat4 model;
}model_pc_data;
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
layout(location = 0) out VSOutput
{
     vec4 fragColor;
     vec2 fragUV;
}vs_out;

out gl_PerVertex
{
    vec4 gl_Position;
};
//------------------------------------------------------------------------------
void main()
{
    // Final position of the vertices
    gl_Position = model_pc_data.model * vec4(inPosition);

    // Out from vertex shader
    vs_out.fragColor    = inColor;
	vs_out.fragUV = inTexCoord;
}
//------------------------------------------------------------------------------
