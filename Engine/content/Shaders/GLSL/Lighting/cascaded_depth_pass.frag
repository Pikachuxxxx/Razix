#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Fragment shader to render a cascaded depth texture onto a Texture2DArray
 */
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
// This extension enables ussage og gl_Layer in the Vertex Shader stage itself instead of using GS 
// also needs VK_EXT_shader_viewport_index_layer device extension enabled and layersCount in VkRenderingInfo
#extension GL_ARB_shader_viewport_layer_array : enable

//------------------------------------------------------------------------------
// Vertex Shader Stage Output
layout(location = 0) in VSOutput{ 
    vec2 texCoord;
} vs_in;
//------------------------------------------------------------------------------
layout(early_fragment_tests) in;
//------------------------------------------------------------------------------
void main() { }