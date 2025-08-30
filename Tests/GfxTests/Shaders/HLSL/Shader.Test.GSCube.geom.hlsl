/*
 * Razix Engine HLSL Geometry Shader File
 * Geometry shader to convert equi-rectangular hdr file to cube map, uses a Geometry shader to generate a cube map
 */
//------------------------------------------------------------------------------
#include <Common/ShaderInclude.Builtin.FrameData.h>
//------------------------------------------------------------------------------
// Vertex Input
// We will use a geom shader to generate the vertices so we don't need any input in this case
struct GSIn
{
    float4 Position: SV_POSITION;
    uint FaceIdx: BLENDINDICES;
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
[maxvertexcount(6)] // 8 points for cube (vertices)
void GS_MAIN(point GSIn input[1], inout TriangleStream<GSOut> Stream)
{
    // Define 8 cube corner points (vertices)
    float3 CubeCorners[8] = {
        float3(0.5, 0.5, 0.5), // +X, +Y, +Z
        float3(0.5, 0.5, -0.5), // +X, +Y, -Z
        float3(0.5, -0.5, 0.5), // +X, -Y, +Z
        float3(0.5, -0.5, -0.5), // +X, -Y, -Z
        float3(-0.5, 0.5, 0.5), // -X, +Y, +Z
        float3(-0.5, 0.5, -0.5), // -X, +Y, -Z
        float3(-0.5, -0.5, 0.5), // -X, -Y, +Z
        float3(-0.5, -0.5, -0.5) // -X, -Y, -Z
    };

    
    // Create triangles using the 8 cube corners (2 triangles per face, but output only 8 vertices total)
    // The idea is to form the cube faces using these vertices and repeat the vertices as needed.
    uint TriangleIndices[6][6] = {
        {0, 2, 1, 2, 3, 1}, // +X face
        {4, 5, 6, 5, 7, 6}, // -X face
        {0, 1, 4, 1, 5, 4}, // +Y face
        {2, 6, 3, 3, 6, 7}, // -Y face
        {0, 4, 2, 2, 4, 6}, // +Z face
        {1, 3, 5, 3, 7, 5}  // -Z face
    };

    // Output the triangles using the same set of 8 vertices, while constructing the cube faces
    // For each face, output 2 triangles
    int faceIdx = input[0].FaceIdx;
    for (int triIdx = 0; triIdx < 2; triIdx++) {
        GSOut gsOutput[3]; // Each triangle has 3 vertices

        // Fetch the indices for the triangle
        uint idx0 = TriangleIndices[faceIdx][triIdx * 3 + 0];
        uint idx1 = TriangleIndices[faceIdx][triIdx * 3 + 1];
        uint idx2 = TriangleIndices[faceIdx][triIdx * 3 + 2];

        gsOutput[0].Position = mul(frame_info.camera.projection, mul(frame_info.camera.view, float4(CubeCorners[idx0], 1.0f)));
        gsOutput[0].RTIndex = faceIdx;
        //gsOutput[0].PointSize = input[0].PointSize;
        gsOutput[0].WorldDir = CubeCorners[idx0];

        gsOutput[1].Position = mul(frame_info.camera.projection, mul(frame_info.camera.view, float4(CubeCorners[idx1], 1.0f)));
        gsOutput[1].RTIndex = faceIdx;
        //gsOutput[1].PointSize = input[0].PointSize;
        gsOutput[1].WorldDir = CubeCorners[idx1];

        gsOutput[2].Position = mul(frame_info.camera.projection, mul(frame_info.camera.view, float4(CubeCorners[idx2], 1.0f)));
        gsOutput[2].RTIndex = faceIdx;
        //gsOutput[2].PointSize = input[0].PointSize;
        gsOutput[2].WorldDir = CubeCorners[idx2];

        // Append the triangle to the stream
        Stream.Append(gsOutput[0]);
        Stream.Append(gsOutput[1]);
        Stream.Append(gsOutput[2]);
        Stream.RestartStrip();
    }
}
