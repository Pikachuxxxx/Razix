/*
 * Razix Engine Shader File
 * Fragment shader to render a skybox as a cubemap
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
 // Vertex Input
 layout(location = 0) in VSOutput
 {
     vec3 fragLocalPos;
 }fs_in;
 //------------------------------------------------------------------------------
 // Fragment Shader Stage Uniforms
//layout(set = 1, binding = 0) uniform samplerCube environmentMap;
layout (push_constant) uniform EnvMap
{
    uint idx;
}tex;

//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
vec3 ACES(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}
// Lottes 2016, "Advanced Techniques and Optimization of HDR Color Pipelines"
vec3 lottes(vec3 x) {
  const vec3 a = vec3(1.6);
  const vec3 d = vec3(0.977);
  const vec3 hdrMax = vec3(8.0);
  const vec3 midIn = vec3(0.18);
  const vec3 midOut = vec3(0.267);

  const vec3 b =
      (-pow(midIn, a) + pow(hdrMax, a) * midOut) /
      ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
  const vec3 c =
      (pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
      ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

  return pow(x, a) / (pow(x, a * d) * b + c);
}
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
void main()
{
    vec3 envColor = texture(global_textures_cubemap[nonuniformEXT(tex.idx)], fs_in.fragLocalPos).rgb;  
    envColor = lottes(envColor);
     // gamma correct
    envColor = pow(envColor, vec3(1.0/2.2)); 
    outSceneColor = vec4(envColor, 1.0);
}
//------------------------------------------------------------------------------
