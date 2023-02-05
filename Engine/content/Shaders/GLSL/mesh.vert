#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Mesh shader used to render meshes
 */
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable

// TODO: Implement this
//#include "binding_table.glsl"

//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;   // this is not needed for all types of meshes, but since we use a common vertex shader we need this too
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;
//------------------------------------------------------------------------------
// Uniforms and Push Constants
// view projection matrix
layout(set = 0, binding = 0) uniform ModelViewProjectionSystemUBO
{
    mat4 model;
    mat4 viewProjection;
} mvp;
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
layout(location = 0) out VSOutput
{
    vec3 fragPos;
    vec4 fragColor;
    vec2 fragTexCoord;
    vec3 fragNormal;
    vec3 fragTangent;
}vs_out;
  
out gl_PerVertex
{
    vec4 gl_Position;
};
//------------------------------------------------------------------------------
void main()
{
    // Final position of the vertices
    gl_Position = mvp.viewProjection * mvp.model * vec4(inPosition, 1.0);

    // Out from vertex shader
    vs_out.fragPos      = vec3(mvp.model * vec4(inPosition, 1.0));
    vs_out.fragColor    = inColor;
	vs_out.fragTexCoord = inTexCoord;
    vs_out.fragNormal   = mat3(transpose(inverse(mvp.model))) * inNormal;
    vs_out.fragTangent  = inTangent;
}
//------------------------------------------------------------------------------
