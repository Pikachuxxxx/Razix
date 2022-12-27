/*
 * Razix Engine Shader File
 * RadianceInjection.geom : Calculate Radiance Injection onto the mesh using Gs
 */
#version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable
 // also needs VK_EXT_shader_viewport_index_layer device extension enabled and layersCount in VkRenderingInfo
#extension GL_ARB_shader_viewport_layer_array : enable

layout(points) in;
layout(location = 0) in VSOutput {
  vec3 N;
  vec4 flux;
  flat ivec3 cellIndex;
}
gs_in[];

layout(points, max_vertices = 1) out;
layout(location = 0) out FragData {
  vec3 N;
  vec4 flux;
}
gs_out;

void main() {
  gl_Position = gl_in[0].gl_Position;
  gl_PointSize = gl_in[0].gl_PointSize;
  gl_Layer = gs_in[0].cellIndex.z;

  gs_out.N = gs_in[0].N;
  gs_out.flux = gs_in[0].flux;

  EmitVertex();
  EndPrimitive();
}