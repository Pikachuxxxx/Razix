/*
 * Razix Engine Shader File
 * RadianceInjection.frag : Calculate Radiance Injection onto the mesh
 */
 #version 450 
  // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable

//------------------------------------------------------------------------------
// Include Files
#include <Lighting/GI/LPV.glsl>
//------------------------------------------------------------------------------
// Gs Input
layout(location = 0) in FragData {
  vec3 N;
  vec4 flux;
}
fs_in;
//------------------------------------------------------------------------------
// RTs
layout(location = 0) out vec4 SH_R;
layout(location = 1) out vec4 SH_G;
layout(location = 2) out vec4 SH_B;
//------------------------------------------------------------------------------

void main()
{
  if (length(fs_in.N) < 0.01) discard;

  const vec4 SH_coeffs = SH_evaluateCosineLobe(fs_in.N) / PI;
  SH_R = SH_coeffs * fs_in.flux.r;
  SH_G = SH_coeffs * fs_in.flux.g;
  SH_B = SH_coeffs * fs_in.flux.b;
}