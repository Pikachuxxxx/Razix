/*
 * Razix Engine Shader File
 * Default Fragment Shader that can be used for rendering basic geometry with versampleColor colors and use a sampleColorture as well
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
// Bindless Textures
#define ENABLE_BINDLESS 1
#include <Common/ShaderInclude.Builtin.BindlessResources.glsl>
//------------------------------------------------------------------------------
// VersampleColor Input
layout(location = 0) in VSOutput
{
    vec2 uv;
}fs_in;

//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
//layout(set = 0, binding = 0) uniform sampler2D renderTarget;

layout (push_constant) uniform FinalRT
{
    uint idx;
}tex;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------
// FXAA: https://www.shadertoy.com/view/4tf3D8

// TODO: use this FXAA instead : https://www.shadertoy.com/view/WtsfDH 

float LinearizeDepth(float depth)
{
  float n = 0.1; // camera z near
  float f = 100.0; // camera z far
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));	
}

vec3 sampleColor(vec2 p)
{
    vec3 col = texture(global_textures_2d[nonuniformEXT(tex.idx)], p).rgb;
    return col;
}

vec3 fxaa(vec2 p)
{
    float FXAA_SPAN_MAX   = 8.0;
    float FXAA_REDUCE_MUL = 1.0 / 8.0;
    float FXAA_REDUCE_MIN = 1.0 / 128.0;

    // 1st stage - Find edge
    vec3 rgbNW = sampleColor(p + vec2(-1., -1.));
    vec3 rgbNE = sampleColor(p + vec2(1., -1.));
    vec3 rgbSW = sampleColor(p + vec2(-1., 1.));
    vec3 rgbSE = sampleColor(p + vec2(1., 1.));
    vec3 rgbM  = sampleColor(p);

    vec3 luma = vec3(0.299, 0.587, 0.114);

    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM, luma);

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float lumaSum   = lumaNW + lumaNE + lumaSW + lumaSE;
    float dirReduce = max(lumaSum * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = 1. / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(vec2(FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX), dir * rcpDirMin)) ;

    // 2nd stage - Blur
    vec3 rgbA = .5 * (sampleColor(p + dir * (1. / 3. - .5)) +
                            sampleColor(p + dir * (2. / 3. - .5)));
    vec3 rgbB = rgbA * .5 + .25 * (sampleColor(p + dir * (0. / 3. - .5)) +
                                        sampleColor(p + dir * (3. / 3. - .5)));

    float lumaB = dot(rgbB, luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    return ((lumaB < lumaMin) || (lumaB > lumaMax)) ? rgbA : rgbB;
}

void main()
{
    vec3 antiAliased = fxaa(fs_in.uv);
    
    outFragColor = vec4(antiAliased, 1.0f);
}
//------------------------------------------------------------------------------
