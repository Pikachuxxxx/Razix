/*
 * Razix Engine HLSL Vertex Shader File
 * Geometry Shader Testing
 * (Pass-through)
 */

//------------------------------------------------------------------------------
// Vertex Input
// We will use a geom shader to generate the vertices so we don't need any input in this case
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
struct VSOut 
{
    float4 Position: SV_POSITION;
    float PointSize: PSIZE;
    uint FaceIdx: BLENDINDICES;
};
//------------------------------------------------------------------------------
// Uniforms and Push Constants
// view projection matrix

//------------------------------------------------------------------------------
VSOut VS_MAIN(uint id : SV_VertexID)
{
    VSOut output;
    output.Position = float4(0.0f, 0.0f, 0.0, 1.0);
    output.PointSize = 16.0f;
    output.FaceIdx = id;
    return output;
}