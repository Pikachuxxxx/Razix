/*
 * Razix Engine GLSL Vertex Shader File
 * Default Vertex Shader that can be used for rendering basic geometry with vertex colors and use a texture as well
 */
//------------------------------------------------------------------------------
#include "../ShaderCommon/ShaderInclude.Builtin.ShaderLangCommon.h"
//------------------------------------------------------------------------------
struct VSIn
{
    float4 color : Color;
};

float4 PS_MAIN(VSIn vsIn) : SV_TARGET
{
    return vsIn.color;
}