/*
 * Razix Engine HLSL Vertex Shader File
 * Used to render a Skybox
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
#include <Common/ShaderInclude.Builtin.FrameData.h>
#include <Common/ShaderInclude.Builtin.VertexInput.h> // define the common vertex input over the engine
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
struct VSOut
{
    float4 Position: SV_POSITION;
    float4 PixelLocalPosTime: POSITION1;
};
//------------------------------------------------------------------------------
VSOut VS_MAIN(VSIn vsIn)
{
    VSOut vs_out;
	vs_out.PixelLocalPosTime.xyz = vsIn.inPosition;
//    vs_out.PixelLocalPosTime.y = -vs_out.PixelLocalPosTime.y;
    vs_out.PixelLocalPosTime.w = frame_info.time;

    float4x4 rotView = frame_info.camera.view;
    rotView[3] = float4(0.0, 0.0, 0.0, 1.0); // Zero out the translation component
    
    float4 cameraPos = mul(rotView, float4(vs_out.PixelLocalPosTime.xyz, 1.0));
    float4 clipPos = mul(frame_info.camera.projection, cameraPos);

    vs_out.Position = clipPos.xyww; // Z = 1 must happen when we do perspective division in clipping phase
    
    return vs_out;
}
//------------------------------------------------------------------------------
