#ifndef _VERTEX_INPUT_HLSL_
#define _VERTEX_INPUT_HLSL_
//----------------------------------------------------------------------------
// TODO: use the engine file to maintain singularity
struct VSIn
{
    float3 inPosition   : POSITION;
    float4 inColor      : COLOR;
    float2 inTexCoord   : TEXCOORD;
    float3 inNormal     : NORMAL;
    float3 inTangent    : TANGENT;
};

#endif
