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
// Constants and Defines
#define SSAO_KERNEL_SIZE 64
const float kInvKernelSize = 1.0 / SSAO_KERNEL_SIZE;
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec2 fragUV;
}fs_in;
//--------------------------------------------------------
// Push constants
//------------------------------------------------------------------------------
// @ slot #0 - .rgb = Normal   .a = Metallic
// @ slot #1 - .rgb = Albedo   .a = Roughness
// @ slot #2 - .rgb = Position .a = AO
layout (set = 0, binding = 0) uniform sampler2D gBuffer0; 
layout (set = 0, binding = 1) uniform sampler2D SceneDepth; 
//--------------------------------------------------------
layout (set = 1, binding = 0) uniform KernelSamples
{
    vec4 samples[SSAO_KERNEL_SIZE];
}Kernel;
layout (set = 1, binding = 1) uniform sampler2D SSAONoiseTex; 
//--------------------------------------------------------
layout (set = 2, binding = 0) uniform SSAOFrameData
{
    float radius;
    float bias;
    vec2 resolution; 
    vec3 camViewPos;
    mat4 viewMatrix;
    mat4 projectionMatrix;
}SSAOData;
//------------------------------------------------------------------------------
// Output from Fragment Shader : Final Render targets 
layout(location = 0) out vec4 outSceneColor;
layout(location = 1) out vec4 outSceneColor1;
//------------------------------------------------------------------------------
vec4 viewToClip(vec4 v) { return SSAOData.projectionMatrix * v; }
vec3 viewToNDC(vec4 v) {
  const vec4 P = viewToClip(v);
  return P.xyz / P.w;
}
vec3 NDCToView(vec4 v) {
  const vec4 P = inverse(SSAOData.projectionMatrix) * v;
  return P.xyz / P.w;
}
//--------------------------------------------------------
// @param z in NDC
// @param uv in range: [0..1]
vec3 viewPositionFromDepth(float z, vec2 uv) {
  // https://stackoverflow.com/questions/11277501/how-to-recover-view-space-position-given-view-space-depth-value-and-ndc-xy/46118945#46118945
  const vec4 ndc = vec4(uv * 2.0 - 1.0, z, 1.0);
  return NDCToView(ndc);
}
//--------------------------------------------------------
void main()
{

    vec2 uv = fs_in.fragUV;
    vec3 viewPos = SSAOData.camViewPos;

    const float depth = texture(SceneDepth, uv).r;
    if (depth >= 1.0f) discard;

    vec4 N_M = texture(gBuffer0, uv);

    const vec2 gBufferSize = textureSize(SceneDepth, 0);
    const vec2 noiseSize = textureSize(SSAONoiseTex, 0);
    const vec2 noiseTexCoord = (gBufferSize / noiseSize) * uv;
    const vec3 rvec = vec3(texture(SSAONoiseTex, noiseTexCoord).xy, 0.0);

    vec3 N = N_M.rgb;
    N = mat3(SSAOData.viewMatrix) * normalize(N);
    const vec3 T = normalize(rvec - N * dot(rvec, N));
    const vec3 B = cross(N, T);
    const mat3 TBN = mat3(T, B, N); // tangent-space -> view-space
    
    const vec3 fragPosViewSpace = viewPositionFromDepth(depth, uv);
    
    float occlusion = 0.0;
    for (uint i = 0; i < SSAO_KERNEL_SIZE; ++i) {
        vec3 samplePos = TBN * Kernel.samples[i].xyz;
        samplePos = fragPosViewSpace + samplePos * SSAOData.radius;
        const vec2 offset = viewToNDC(vec4(samplePos, 1.0)).xy * 0.5 + 0.5;
        float sampleDepth = texture(SceneDepth, offset).r;
        sampleDepth = viewPositionFromDepth(sampleDepth, offset).z;

        const float rangeCheck =
        smoothstep(0.0, 1.0, SSAOData.radius / abs(fragPosViewSpace.z - sampleDepth));
        occlusion +=
        (sampleDepth >= samplePos.z + SSAOData.bias ? 1.0 : 0.0) * rangeCheck;
    }
     // 1.0 = no occlusion, 0.0 = occluded
    occlusion = 1.0 - occlusion * kInvKernelSize;
    
    outSceneColor = vec4(vec3(occlusion), 1.0f);
    outSceneColor1 = vec4(vec3(occlusion), 1.0f);

}