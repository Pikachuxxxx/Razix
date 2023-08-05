#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Mesh shader used to render meshes and select the RT layer in a multi layered RT
 */
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
// This extension is for #include support in glsl, this extension is usually not supported to force enable it perhaps?
//#extension GL_ARB_shading_language_include : enable

// This extension enables ussage og gl_Layer in the Vertex Shader stage itself instead of using GS 
// also needs VK_EXT_shader_viewport_index_layer device extension enables and layersCount in VkRenderingInfo
#extension GL_ARB_shader_viewport_layer_array : enable

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
// view projection matrix
layout(set = 0, binding = 0) uniform ModelViewProjectionSystemUBO
{
    mat4 model;
    mat4 view;
	mat4 proj;
    int layerIdx;
} mvp;
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
layout(location = 0) out VSOutput
{
    vec3 fragPos;
    vec4 fragColor;
    vec2 fragUV;
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
    // Select the layer for rendering onto the Render Target layer
    gl_Layer = mvp.layerIdx;
    // Final position of the vertices
    gl_Position = mvp.proj * mvp.view * mvp.model * vec4(inPosition, 1.0);

    // Out from vertex shader
    vs_out.fragPos      = vec3(mvp.model * vec4(inPosition, 1.0));
    vs_out.fragColor    = inColor;
	vs_out.fragUV = inTexCoord;
    vs_out.fragNormal   = mat3(transpose(inverse(mvp.model))) * inNormal;
    vs_out.fragTangent  = inTangent;
}
//------------------------------------------------------------------------------
