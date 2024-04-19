/*
 * Razix Engine GLSL Pixel Shader File
 * Applies tonemapping to the final scene HDR render target
 */
#version 450 core
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
#include <FX/ShaderInclude.Builtin.Tonemapping.glsl>
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec2 uv;
}fs_in; 
//--------------------------------------------------------
// Push constants
//------------------------------------------------------------------------------
layout (set = 0, binding = 0) uniform sampler2D SceneHDRRenderTarget; 
//--------------------------------------------------------
layout (push_constant) uniform PushConstantData{
    uint tonemapMode;
}pcData;
//------------------------------------------------------------------------------
// Output from Fragment Shader : Final Render targets 
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
void main()
{
    vec3 result = texture(SceneHDRRenderTarget, fs_in.uv).rgb;
    
    // Tonemap
    switch(pcData.tonemapMode)
    {
        case 0:
            result = ACES(result);
            break;
        case 1:
            result = Filmic(result);
            break;
        case 2:
            result = lottes(result);
            break;
        case 3:
            result = reinhard(result);
            break;
        case 4:
            result = reinhard2(result);
            break;
        case 5:
            result = uchimura(result);
            break;
        case 6:
            result = uncharted2(result);
            break;
        case 7:
            result = unreal(result);
            break;
        case 8:
            result = result;
            break;
    }

    outSceneColor = vec4(result, 1.0f);
}