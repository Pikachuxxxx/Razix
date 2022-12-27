/*
 * Razix Engine Shader File
 * RadianceInjection.vert : Calculate Radiance Injection onto the mesh
 */
 #version 450 core
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
#include <Lighting/GI/LPV.glsl>
//------------------------------------------------------------------------------
// Bindings (UBOs and Samplers)
layout(set = 0, binding = 0) uniform RadianceInjectionUBO
{
    int RSMResolution;
    vec3 MinCorner;
    vec3 GridSize;
    float CellSize;
}radianceInjectionData;

layout(set = 0, binding = 1) uniform sampler2D worldPosRT;
layout(set = 0, binding = 2) uniform sampler2D worldNormalRT;
layout(set = 0, binding = 3) uniform sampler2D fluxRT;
//------------------------------------------------------------------------------
// Output Data to Gs/Ps
layout(location = 0) out VSOutput
{
    vec3 N;
    vec4 flux;
    flat ivec3 cellIndex;
}vs_out;
//------------------------------------------------------------------------------

void main()
{
    const ivec2 coord = ivec2(unflatten2D(gl_VertexIndex, radianceInjectionData.RSMResolution));

    const vec3 position = texelFetch(worldPosRT, coord, 0).xyz;
    const vec3 N = texelFetch(worldNormalRT, coord, 0).xyz;
    const vec4 flux = texelFetch(fluxRT, coord, 0);

    vs_out.N = N;
    vs_out.flux = flux;

    const ivec3 gridCell = ivec3((position - radianceInjectionData.MinCorner) / radianceInjectionData.CellSize + 0.5 * N);
    vs_out.cellIndex = gridCell;

    const vec2 ndc = (vec2(gridCell.xy) + 0.5) / radianceInjectionData.GridSize.xy * 2.0 - 1.0;
    gl_Position = vec4(ndc, 0.0, 1.0);

    gl_PointSize = 1.0;
}