/*
 * Razix Engine GLSL Pixel Shader File
 * Calcualte the Final PBR direct lighting 
 */
#version 450 core
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
#include <Math/ShaderInclude.Builtin.GaussianTapFilters.glsl>
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec2 fragUV;
}fs_in; 
//--------------------------------------------------------
// Push constants
//------------------------------------------------------------------------------
layout (set = 0, binding = 0) uniform sampler2D inputTexture; 
//--------------------------------------------------------
layout (push_constant) uniform PushConstantData {
    uint tapFilter;
    float blurRadius;
    vec2 direction;
}pc_data;
//------------------------------------------------------------------------------
// Output from Fragment Shader : Final Render targets 
layout(location = 0) out vec4 outSceneColor;
//--------------------------------------------------------
void main()
{
    vec2 uv = fs_in.fragUV;
    vec2 resolution = textureSize(inputTexture, 0);

    switch(pc_data.tapFilter) {
        case FiveTap:
            outSceneColor = blur5(inputTexture, uv, resolution, pc_data.direction);
            break;
        case NineTap:
            outSceneColor = blur9(inputTexture, uv, resolution, pc_data.direction);
            break;
        case ThirteenTap:
            outSceneColor = blur13(inputTexture, uv, resolution, pc_data.direction);
            break;
    }    
}