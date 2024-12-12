/*
 * Razix Engine HLSL Geometry Shader File
 * Geometry shader to convert equi-rectangular hdr file to cube map, uses a Geometry shader to generate a cube map
 */

//------------------------------------------------------------------------------
// Vertex Input
// We will use a geom shader to generate the vertices so we don't need any input in this case
struct GSIn
{
    float4 Position: SV_POSITION;
};
//------------------------------------------------------------------------------
// Geometry Shader Stage Output

struct GSOut
{
    float4 Position : SV_POSITION;                  // Final screen-space position
    uint   RTIndex  : SV_RenderTargetArrayIndex;    // Cubemap face index
    float3 WorldDir : TEXCOORD0;                    // Cubemap direction
};
//------------------------------------------------------------------------------
cbuffer ViewProjectionSystemUBO: register(b0, space0)
{
    float4x4 view;
	float4x4 proj;
};
//------------------------------------------------------------------------------
[maxvertexcount(36)] // 6 faces * 4 vertices per face
void GS_MAIN(triangle GSIn input[3], inout TriangleStream<GSOut> Stream)
{
     // Define the render target index for cube map faces
    static const int faceIndices[6] = { 0, 1, 2, 3, 4, 5 };

    // Cube vertices in model space for each face
    static const float3 cubeVerts[8] = {
        float3(-1, -1, -1), float3( 1, -1, -1),
        float3(-1,  1, -1), float3( 1,  1, -1),
        float3(-1, -1,  1), float3( 1, -1,  1),
        float3(-1,  1,  1), float3( 1,  1,  1)
    };

    static const uint indices[36] = {
        0, 1, 2,  2, 1, 3, // -Z face
        4, 6, 5,  5, 6, 7, // +Z face
        0, 2, 4,  4, 2, 6, // -X face
        1, 5, 3,  3, 5, 7, // +X face
        0, 4, 1,  1, 4, 5, // -Y face
        2, 3, 6,  6, 3, 7  // +Y face
    };

    // Loop through cube faces (6 in total)
    for (int face = 0; face < 6; ++face)
    {
        for (int i = 0; i < 6; i += 3) // Each face has 2 triangles (6 vertices)
        {
            GSOut verts[3];
            for (int j = 0; j < 3; ++j)
            {
                float3 pos = cubeVerts[indices[face * 6 + i + j]];
                //float4 viewSpacePos = mul(float4(pos, 1.0), view);
                verts[j].Position = float4(pos, 1.0);
                verts[j].WorldDir = normalize(pos); // Use position as normal
                verts[j].RTIndex = faceIndices[face]; // Assign target index
            }
            Stream.Append(verts[0]);
            Stream.Append(verts[1]);
            Stream.Append(verts[2]);
            Stream.RestartStrip();
        }
    }
}