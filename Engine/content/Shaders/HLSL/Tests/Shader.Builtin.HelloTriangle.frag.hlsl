/*
 * Razix Engine GLSL Vertex Shader File
 * Default Vertex Shader that can be used for rendering basic geometry with vertex colors and use a texture as well
 */
//------------------------------------------------------------------------------
#include "../../ShaderCommon/ShaderInclude.Builtin.ShaderLangCommon.h"
//------------------------------------------------------------------------------

//#include "./Math/ShaderInclude.Builtin.Patterns.h"


struct PSIn
{
    float4 Position : SV_POSITION;
    float4 Color    : COLOR0;
};
 //------------------------------------------------------------------------------
float4 PS_MAIN(PSIn psIn) : SV_TARGET
{
    // Add grid-like texture pattern on top of the color
    //float texP = Patterns::patternGrid(psIn.Position.xy, 0.15);
    return psIn.Color;
}