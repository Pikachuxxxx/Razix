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
#define DISABLE_FRAME_DATA_BINDING
#include <Common/ShaderInclude.Builtin.FrameData.glsl>
//------------------------------------------------------------------------------
// Constants and Defines
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec2 fragUV;
    FrameInfo info;
}fs_in;
//--------------------------------------------------------
// Push constants
//------------------------------------------------------------------------------
// @ slot #0 - .rgb = Normal   .a = Metallic
// @ slot #1 - .rgb = Albedo   .a = Roughness
// @ slot #2 - .rgb = Position .a = AO
layout (set = 1, binding = 0) uniform sampler2D gBuffer0; 
layout (set = 1, binding = 1) uniform sampler2D gBuffer1; 
layout (set = 1, binding = 2) uniform sampler2D gBuffer2; 
//--------------------------------------------------------
const int SSAO_KERNEL_SIZE = 64;
layout (set = 2, binding = 0) uniform KernelSamples
{
    vec4 samples[SSAO_KERNEL_SIZE];
}Kernel;
layout (set = 2, binding = 1) uniform sampler2D SSAONoiseTex; 
//------------------------------------------------------------------------------
// Output from Fragment Shader : Final Render targets 
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
void main()
{
    vec2 uv = fs_in.fragUV;
    vec3 viewPos = getCameraPosition(fs_in.info.camera);

    vec4 N_M = texture(gBuffer0, uv);
    vec4 A_R = texture(gBuffer1, uv);
    vec4 P_O = texture(gBuffer2, uv);

    // Calculate Normal and fragPos in View Space as SSAO is a screen-space technique where occlusion is calculated from the visible view
    vec3 fragPosVS  = (vec4(P_O.rgb, 1.0f)).rgb;
    vec3 N_VS       = (vec4(N_M.rgb, 1.0f)).rgb;

    vec2 screenSize = fs_in.info.resolution;
    const vec2 noiseScale = screenSize / 4.0f;

    vec3 randomVec = texture(SSAONoiseTex, uv * noiseScale).rgb * 2.0f - 1.0f;
    vec3 tangent   = normalize(randomVec - N_VS * dot(randomVec, N_VS));
    vec3 bitangent = cross(tangent, N_VS);
    mat3 TBN       = mat3(tangent, bitangent, N_VS); 

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * Kernel.samples[i].xyz; // from tangent to view-space
        samplePos = fragPosVS + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = fs_in.info.camera.projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = texture(gBuffer2, offset.xy).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPosVS.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize); 
}