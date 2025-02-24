/*
 * Razix Engine HLSL Pixel Shader File
 * Calculate the Final PBR direct lighting 
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
// GBuffer Deferred bindings
#include <Rendering/ShaderInclude.Builtin.DeferredPassBindings.h>
//-------------------------------
// PBR - BRDF helper functions
//#include <Lighting/PBR/ShaderInclude.Builtin.DisneyBRDF.h>
//-------------------------------
// Color Utils (Debug only)
//#include <Utils/ShaderInclude.Builtin.Color.h>
//------------------------------------------------------------------------------
// Final Render targets 
struct PSOut
{
    float4 SceneColor: SV_Target0;
};
//------------------------------------------------------------------------------
PSOut PS_MAIN(VSOutput input)
{
    // Flip UVs since the quad is also flipped and so are it's UVs
    float2 uv = float2(input.UV.x, 1.0f - input.UV.y);

    PSOut output;
    output.SceneColor = gBuffer0.Sample(linearSampler, uv);

    return output;
}