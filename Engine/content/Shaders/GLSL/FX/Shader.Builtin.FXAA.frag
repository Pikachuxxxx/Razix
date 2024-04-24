/*
 * Razix Engine GLSL Pixel Shader File
 * Resolves TAA and write to the history buffer
 */
#version 450 core
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
#include <Utils/ShaderInclude.Builtin.Color.glsl>
//------------------------------------------------------------------------------
// Constants and Defines
#define FXAA_SPAN_MAX 64.0
#define FXAA_REDUCE_MUL (1.0 / FXAA_SPAN_MAX)
#define FXAA_REDUCE_MIN (1.0 / 128.0)
#define FXAA_SUBPIX_SHIFT (1.0 / 32.0)
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec2 uv;
}fs_in;
//--------------------------------------------------------
// Push Constants
layout (push_constant) uniform PushConstantData{
    vec2 resolution;
}pcData;
//--------------------------------------------------------
layout (set = 0, binding = 0) uniform sampler2D SceneTexture; 
//------------------------------------------------------------------------------
// Output from Fragment Shader : Write to an accumulation buffer
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
void main()
{
    const vec2 texelSize = vec2(1.0f, 1.0f) / pcData.resolution;
    const vec2 uv2 = fs_in.uv;
    const vec4 uv = vec4(uv2, uv2 - (texelSize * (0.5 + FXAA_SUBPIX_SHIFT)));

    // 1st stage - Find edge
    const vec3 rgbNW = textureLod(SceneTexture, uv.zw, 0.0).xyz;
    const vec3 rgbNE = textureLod(SceneTexture, uv.zw + vec2(1.0, 0.0) * texelSize.xy, 0.0).xyz;
    const vec3 rgbSW = textureLod(SceneTexture, uv.zw + vec2(0.0, 1.0) * texelSize.xy, 0.0).xyz;
    const vec3 rgbSE = textureLod(SceneTexture, uv.zw + vec2(1.0, 1.0) * texelSize.xy, 0.0).xyz;
    const vec3 rgbM = textureLod(SceneTexture, uv.xy, 0.0).xyz;

    const float lumaNW = getLuminance(rgbNW);
    const float lumaNE = getLuminance(rgbNE);
    const float lumaSW = getLuminance(rgbSW);
    const float lumaSE = getLuminance(rgbSE);
    const float lumaM = getLuminance(rgbM);

    const float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    const float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    const float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    const float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * texelSize.xy;

    // 2nd stage - Blur
    const vec3 rgbA = (1.0 / 2.0) *
    (textureLod(SceneTexture, uv.xy + dir * (1.0 / 3.0 - 0.5), 0.0).xyz +
    textureLod(SceneTexture, uv.xy + dir * (2.0 / 3.0 - 0.5), 0.0).xyz);

    const vec3 rgbB = rgbA * (1.0 / 2.0) + (1.0 / 4.0) *
    (textureLod(SceneTexture, uv.xy + dir * (0.0 / 3.0 - 0.5), 0.0).xyz +
    textureLod(SceneTexture, uv.xy + dir * (3.0 / 3.0 - 0.5), 0.0).xyz);

    const float lumaB = getLuminance(rgbB);

    vec3 color = (lumaB < lumaMin) || (lumaB > lumaMax) ? rgbA : rgbB;

    outSceneColor = vec4(color, 1.0f);
}