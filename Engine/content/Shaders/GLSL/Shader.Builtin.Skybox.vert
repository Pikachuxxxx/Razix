#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Used to render a Skybox
 */
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
#include <Common/ShaderInclude.FrameData.glsl>
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor; 
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
layout(location = 0) out VSOutput
{
    vec3 fragLocalPos;
    float time;
}vs_out;

out gl_PerVertex
{
    vec4 gl_Position;
};
//------------------------------------------------------------------------------
void main()
{
	vs_out.fragLocalPos = inPosition;
    vs_out.fragLocalPos.y = -vs_out.fragLocalPos.y;
    vs_out.time = u_Frame.time;

    mat4 rotView = mat4(mat3(u_Frame.camera.view)); // remove translation from the view matrix
    vec4 clipPos = u_Frame.camera.projection * rotView * vec4(vs_out.fragLocalPos, 1.0);

    gl_Position = clipPos.xyww;
}
//------------------------------------------------------------------------------
