/*
 * Razix Engine HLSL Fragment Shader File
 * Visibility Buffer: To pack both SV_PrimitiveID and InstanceID into a 32-bit texture, these is used later to interpolate and shade pixels
 */
//------------------------------------------------------------------------------
#include "../../../ShaderCommon/ShaderInclude.Builtin.ShaderLangCommon.h"
//------------------------------------------------------------------------------
#include "../../Math/ShaderInclude.Builtin.PackingUtils.h"

#include "ShaderInclude.Builtin.VisibilityBufferCommon.h"
//------------------------------------------------------------------------------
struct PSIn
{
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
};
 //------------------------------------------------------------------------------
float4 PS_MAIN(PSIn input, uint primivitveID : SV_PrimitiveID, uint instanceID : SV_InstanceID) : SV_TARGET
{
    // only opaque geometry for now
    // Using instance ID for now instead of drawID
    uint VisBufferEncoding = PACK_VISIBILITY_BUFFER_DATA(primivitveID, instanceID, false);
    // unpack and store in a 8:8:8:8 UNORM buffer, using a 4 channel RT is faster and color compressed
    // and is optimized for faster GPU access, where as a R32 texture will result in full data reads and is slower
    return unpackUnorm4x8(VisBufferEncoding);
}