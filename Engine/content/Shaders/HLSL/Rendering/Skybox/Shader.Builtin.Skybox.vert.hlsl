/*
 * Razix Engine HLSL Vertex Shader File
 * Used to render a Skybox
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
#include <Common/ShaderInclude.Builtin.FrameData.h>
#include <Razix/Shared/RZShaderCompitability.h>    // define the common vertex input over the engine
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
struct VSOut
{
    float4 Position : SV_POSITION;
    float4 PixelLocalPosTime : POSITION1;
};
//------------------------------------------------------------------------------
VSOut VS_MAIN(VSIn vsIn)
{
    VSOut vs_out;
    vs_out.PixelLocalPosTime.xyz = vsIn.Position;
    //    vs_out.PixelLocalPosTime.y = -vs_out.PixelLocalPosTime.y;
    vs_out.PixelLocalPosTime.w = frame_info.time;

    float4x4 rotView = frame_info.camera.view;
    rotView[0][3]    = 0.0;
    rotView[1][3]    = 0.0;
    rotView[2][3]    = 0.0;
    rotView[3][3]    = 1.0;

    float4 cameraPos = mul(rotView, float4(vs_out.PixelLocalPosTime.xyz, 1.0));
    float4 clipPos   = mul(frame_info.camera.projection, cameraPos);

    vs_out.Position = clipPos.xyww;    // Z = 1 must happen when we do perspective division in clipping phase

    return vs_out;
}
//------------------------------------------------------------------------------
