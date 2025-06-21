/*
 * Razix Engine HLSL Vertex Shader File
 * Vertex shader to convert equi-rectangular hdr file to cube map, uses a Geometry shader to generate a cube map
 * (Pass-through)
 */

//------------------------------------------------------------------------------
// Vertex Input
// We will use a geom shader to generate the vertices so we don't need any input in this case
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
struct VSOut
{
    float4 Position : SV_POSITION;
};
//------------------------------------------------------------------------------
// Uniforms and Push Constants
// view projection matrix

//------------------------------------------------------------------------------
VSOut VS_MAIN(uint id : SV_VertexID)
{
    VSOut output;
    output.Position = float4(0.0f, 0.0f, 0.0, 1.0);
    return output;
}