/*
 * Razix Engine Shader File
 * RadiancePropagation.vert
 */
 #version 450 core
  // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable
 #extension GL_ARB_draw_instanced : enable

//------------------------------------------------------------------------------
#include <Common/ShaderInclude.Math.glsl>
//------------------------------------------------------------------------------
// Bindings
layout(set = 0, binding = 0) uniform RadiancePropagationUBO
{
    vec3 GridSize;
}radiancePropagationData;
//------------------------------------------------------------------------------
// Output to Geom Shader
layout(location = 0) out VSOutput
{ 
    flat ivec3 cellIndex;
}vs_out;
//------------------------------------------------------------------------------
void main() {
    const vec3 position = vec3(unflatten3D(gl_VertexIndex, uvec2(radiancePropagationData.GridSize.xy)));

    vs_out.cellIndex = ivec3(position);

    const vec2 ndc = (position.xy + 0.5) / radiancePropagationData.GridSize.xy * 2.0 - 1.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
    // Not needed here done once in Radiance Injection
    //gl_Position.y = -gl_Position.y;	
    gl_PointSize = 1.0;
}
