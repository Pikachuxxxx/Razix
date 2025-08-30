#pragma once

#if !defined(__cplusplus) || (defined(__GLSL__) || defined(__SHADER__) || defined(__PSSL__) || defined(__MSL__) || defined(__SPIRV__) || defined(__LLVM__) || defined(__CUDA__))
// Since HLSL is our main source shader language, it's fine to be HLSL centric
    #define SHADER_SEMANTIC(semantic) : semantic
    #define SHADER_STRUCT struct
#else
    #define SHADER_SEMANTIC(semantic)
    #define SHADER_STRUCT struct
jgdfasladsljkhfghjldsg
    #include "Razix/Core/RZDataTypes.h"
    #include <vector>
#endif

#define MAX_BONES_COUNT                          0x00000004
#define SIMPLE_VERTEX_ATTRIBS_COUNT              0x00000002
#define VERTEX_ATTRIBS_COUNT                     0x00000005
#define VERTEX_ATTRIBS_POS_IDX                   0x00000000
#define VERTEX_ATTRIBS_COL_IDX                   0x00000001
#define VERTEX_ATTRIBS_UV_IDX                    0x00000002
#define VERTEX_ATTRIBS_NOR_IDX                   0x00000003
#define VERTEX_ATTRIBS_TAN_IDX                   0x00000004
#define SKELETAL_VERTEX_ATTRIBS_POS_IDX          0x00000000
#define SKELETAL_VERTEX_ATTRIBS_UV_IDX           0x00000001
#define SKELETAL_VERTEX_ATTRIBS_NOR_IDX          0x00000002
#define SKELETAL_VERTEX_ATTRIBS_TAN_IDX          0x00000003
#define SKELETAL_VERTEX_ATTRIBS_BI_TAN_IDX       0x00000004
#define SKELETAL_VERTEX_ATTRIBS_BONE_INDICES_IDX 0x00000005
#define SKELETAL_VERTEX_ATTRIBS_BONE_WEIGHTS_IDX 0x00000006
#define SKELETAL_VERTEX_ATTRIBS_COUNT            0x00000007
#define VERTEX_MAX_ATTRIBS_COUNT                 SKELETAL_VERTEX_ATTRIBS_COUNT

namespace Razix {
    namespace Gfx {

#ifdef __cplusplus

        struct RZVertex
        {
            std::vector<float3> Position;
            std::vector<float4> Color;
            std::vector<float2> UV;
            std::vector<float3> Normal;
            std::vector<float3> Tangent;
        };

        struct RZSkeletalVertex
        {
            std::vector<float3> Position;
            std::vector<float2> UV;
            std::vector<float3> Normal;
            std::vector<float3> Tangent;
            std::vector<float3> BiTangent;
            std::vector<int4>   BoneIndices;    // MAX_BONES_COUNT
            std::vector<float3> BoneWeights;
        };

        struct RZParticleVertex
        {
            std::vector<float3> Position;
            std::vector<float3> Velocity;
            std::vector<float4> Color;
            std::vector<float2> Size;
            std::vector<float2> Life;
        };

        struct RZTerrainVertex
        {
            std::vector<float3> Position;
            std::vector<float2> UV;
            std::vector<float3> Normal;
            std::vector<float>  Height;
            std::vector<float4> BlendWeights;
        };
#endif
    }    // namespace Gfx
}    // namespace Razix

 // Simple vertex for basic geometry (quads, etc.)
SHADER_STRUCT RZSimpleVertex
{
    float4 Position  SHADER_SEMANTIC(POSITION);
    float2 TexCoords SHADER_SEMANTIC(TEXCOORD0);
};

// Standard vertex input for most meshes
SHADER_STRUCT VSIn
{
    float3 Position SHADER_SEMANTIC(POSITION);
    float4 Color    SHADER_SEMANTIC(COLOR);
    float2 UV       SHADER_SEMANTIC(TEXCOORD0);
    float3 Normal   SHADER_SEMANTIC(NORMAL);
    float3 Tangent  SHADER_SEMANTIC(TANGENT);
};

// Skeletal vertex input for skinned meshes
SHADER_STRUCT RZSkeletalVertexInput
{
    float3 Position    SHADER_SEMANTIC(POSITION);
    float2 UV          SHADER_SEMANTIC(TEXCOORD0);
    float3 Normal      SHADER_SEMANTIC(NORMAL);
    float3 Tangent     SHADER_SEMANTIC(TANGENT);
    float3 BiTangent   SHADER_SEMANTIC(BITANGENT);
    int4 BoneIndices   SHADER_SEMANTIC(BLENDINDICES);
    float4 BoneWeights SHADER_SEMANTIC(BLENDWEIGHT);
};

// Particle vertex input
SHADER_STRUCT RZParticleVertexInput
{
    float3 Position SHADER_SEMANTIC(POSITION);
    float3 Velocity SHADER_SEMANTIC(TEXCOORD0);
    float4 Color    SHADER_SEMANTIC(COLOR);
    float2 Size     SHADER_SEMANTIC(TEXCOORD1);
    float2 Life     SHADER_SEMANTIC(TEXCOORD2);    // current, max
};

// Terrain vertex input
SHADER_STRUCT RZTerrainVertexInput
{
    float3 Position     SHADER_SEMANTIC(POSITION);
    float2 UV           SHADER_SEMANTIC(TEXCOORD0);
    float3 Normal       SHADER_SEMANTIC(NORMAL);
    float Height        SHADER_SEMANTIC(TEXCOORD1);
    float4 BlendWeights SHADER_SEMANTIC(TEXCOORD2);    // for texture blending
};

// Grass/Foliage vertex input
SHADER_STRUCT RZGrassVertexInput
{
    float3 Position SHADER_SEMANTIC(POSITION);
    float2 UV       SHADER_SEMANTIC(TEXCOORD0);
    float3 Normal   SHADER_SEMANTIC(NORMAL);
    float2 WindData SHADER_SEMANTIC(TEXCOORD1);    // wind strength, phase
    float Height    SHADER_SEMANTIC(TEXCOORD2);
};

SHADER_STRUCT RZUIVertexInput
{
    float2 Position SHADER_SEMANTIC(POSITION);
    float2 UV       SHADER_SEMANTIC(TEXCOORD0);
    float4 Color    SHADER_SEMANTIC(COLOR);
};

#undef SHADER_SEMANTIC
#undef SHADER_STRUCT
#undef SHADER_CBUFFER