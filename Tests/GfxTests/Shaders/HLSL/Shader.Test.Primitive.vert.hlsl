/*
 * Razix Engine HLSL Vertex Shader File
 * Shader.Test.Primitive.vert.hlsl : Renders an instanced cube using per-instance model matrices
 * Outputs world-space normal so the pixel shader can visualize it as color.
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
// Resource Bindings
// Instance transform buffer (array of float4x4 model matrices). Bound as SRV.
#define MAX_INSTANCES 16
cbuffer InstanceTransforms : register(b0, space0)
{
    float4x4 view;
    float4x4 projection;
    float4x4 g_InstanceTransforms[MAX_INSTANCES];
};
//------------------------------------------------------------------------------
struct VSIn
{
    float3 Position : POSITION0;
    float3 Normal   : NORMAL0;
};

struct VSOut
{
    float4 Position : SV_POSITION;
    float3 NormalWS : NORMAL0;
};
//------------------------------------------------------------------------------
VSOut VS_MAIN(VSIn vin, uint instanceID : SV_InstanceID)
{
    VSOut vout;

    float4x4 model = g_InstanceTransforms[instanceID];

    float4 worldPos = mul(model, float4(vin.Position, 1.0f));

    float4 viewPos = mul(view, worldPos);
    vout.Position  = mul(projection, viewPos);

    float3 worldNormal = mul((float3x3)model, vin.Normal);
    vout.NormalWS = normalize(worldNormal);

    return vout;
}
