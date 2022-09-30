#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Mesh shader used to render meshes
 */
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
// This extension is for #include support in glsl, this extension is usually not supported to force enable it perhaps?
//#extension GL_ARB_shading_language_include : enable

// TODO: Implement this
//#include "binding_table.glsli"

//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;   // this is not needed for all types of meshes, but since we use a common vertex shader we need this too
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;
//------------------------------------------------------------------------------
// Uniforms and Push Constants
// Thew view projection matrix
layout(set = 0, binding = 0) uniform ViewProjectionSystemUBO
{
    mat4 view;
	mat4 proj;
} view_proj_ubo;

// The model push constant
layout (push_constant) uniform ModelPushConstantData{
    mat4 model;
     int ID;
}model_pc_data;
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
layout(location = 0) out VSOutput
{
    vec3 fragPos;
    vec4 fragColor;
    vec2 fragTexCoord;
    vec3 fragNormal;
    vec3 fragTangent;
    flat int ID;
}vs_out;
  
out gl_PerVertex
{
    vec4 gl_Position;
};
//------------------------------------------------------------------------------
void main()
{
    // Final position of the vertices
    gl_Position = view_proj_ubo.proj * view_proj_ubo.view * model_pc_data.model * vec4(inPosition, 1.0);

    // Out from vertex shader
    vs_out.fragPos      = vec3(model_pc_data.model * vec4(inPosition, 1.0));
    vs_out.fragColor    = inColor;
	vs_out.fragTexCoord = inTexCoord;
    vs_out.fragNormal   = mat3(transpose(inverse(model_pc_data.model))) * inNormal;
    vs_out.fragTangent  = inTangent;
    // Okay for some reason we can't pass integers from VS to PS from the out semantic?
    // This is super weird hence we use this hack of passing a float vec2 insted for some reason float also doesn't work so
    // This needs more investigation so as to why this is happening
    vs_out.ID        = model_pc_data.ID;//model_pc_data.ID;
}
//------------------------------------------------------------------------------
