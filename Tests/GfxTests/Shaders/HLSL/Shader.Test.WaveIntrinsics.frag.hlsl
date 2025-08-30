/*
 * Razix Engine GLSL Fragment Shader File
 * Tesitng WaveIntrinsice in Vk/DX12
 * [Source]: https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/Samples/Desktop/D3D12SM6WaveIntrinsics
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
#include <Math/ShaderInclude.Builtin.Patterns.h>
//------------------------------------------------------------------------------
struct PSIn
{
    float4 Position : SV_POSITION;
    float4 Color    : COLOR0;
};
//------------------------------------------------------------------------------
cbuffer g_WaveIntrinsicsConstantBuffer : register (b0, space0)
{
    uint waveMode;
};
//------------------------------------------------------------------------------
float4 PS_MAIN(PSIn psIn) : SV_TARGET
{
    float4 outputColor;
    uint laneSize = WaveGetLaneCount();

    // Add grid-like texture pattern
    float texP = Patterns::patternGrid(psIn.Position.xy, 0.15);
    outputColor = texP * psIn.Color;
    // https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/shader-model-6-0
    switch(waveMode) 
    {
        case 1: // render as-is
        {
            break; 
        }
        case 2: // WaveIsFirstLane
        {
            // https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/waveisfirstlane
            if(WaveIsFirstLane()) // basically means if it's the first thread in the wavefront (same as SV_GroupThreadID.xy == 0 in a CS)
                outputColor = float4(1, 1, 1, 1);
            break;
        }
        case 3: // WaveGetLaneCount
        {
            // Gradient of colors: ratio of lane index to total, what pixes gets what lane can be visualized here
            float col = WaveGetLaneIndex() / float(laneSize);
            outputColor = float4(col, col, col, 1);
            break;
        }
        case 4: // WaveActiveMax
        {
            // Mark the first active lane as white pixel. Mark the last active lane as red pixel.
            if (WaveIsFirstLane())
                outputColor = float4(1, 1, 1, 1);
            if (WaveGetLaneIndex() == WaveActiveMax(WaveGetLaneIndex()))
                outputColor = float4(1, 0, 0, 1);
        }
        case 5: // WaveActiveBallot
        {
            // We are calculating the Occupancy ratio = (# of active lanes / # of lanes)
            uint4 activeLaneMask = WaveActiveBallot(true);
            uint numActiveLanes = countbits(activeLaneMask.x) + countbits(activeLaneMask.y) + countbits(activeLaneMask.z) + countbits(activeLaneMask.w);
            float activeRatio = (float)numActiveLanes / float(laneSize);
            outputColor = float4(activeRatio, activeRatio, activeRatio, 1.0);
            break;
        }
        case 6: // WaveReadLaneFirst
        {
            // transmits the outputColor of the first thread/lane to all the threads in the wave
            // Paint the entire wave with the same color as of the first lane/thread 
            // shares the expression of the minimum active lane
            outputColor = WaveReadLaneFirst(outputColor);
            break;
        }
        case 7: // WaveActiveSum
        {   
            // typically used to average out values across a wave, 
            // this is similar to using a groupshared variable to record data 
            // and average and sync it with GroupMemoryBarrierWithGroupSync
            // except this is faster and saves precious r/w cycles and memory of LDS
            uint4 activeLaneMask = WaveActiveBallot(true);
            uint numActiveLanes = countbits(activeLaneMask.x) + countbits(activeLaneMask.y) + countbits(activeLaneMask.z) + countbits(activeLaneMask.w);
            float4 avgColor = WaveActiveSum(outputColor) / float(numActiveLanes);
            outputColor = avgColor; // no need a resource barrier sync, this is summed for every lane
            break;
        }    
        case 8: // WavePrefixSum
        {
            // we compute the cumulative sum of a value across all active lanes in the wave
            float4 prefixSum = WavePrefixSum(psIn.Position);
            uint4 activeLaneMask = WaveActiveBallot(true);
            uint numActiveLanes = countbits(activeLaneMask.x) + countbits(activeLaneMask.y) + countbits(activeLaneMask.z) + countbits(activeLaneMask.w);

            outputColor = prefixSum / numActiveLanes;

        }
        case 9: // QuadReadAcross(X/Y): Reads values from neighboring threads within a wave
        {
            // we can read 4 values at a time, typically used in tiled lighting to read up to 4lights at a time per teil
            // For this example we shade each quad with a diff color
            // Color pixels based on their quad id:
            //  q0 -> red
            //  q1 -> green
            //  q2 -> blue
            //  q3 -> white
            //
            // --------------->
            // | [q0] [q1]
            // | [q2] [q3]
            
            float dx = QuadReadAcrossX(psIn.Position.x) - psIn.Position.x;
            float dy = QuadReadAcrossY(psIn.Position.y) - psIn.Position.y;

            // q0
            if (dx > 0 && dy > 0)
                outputColor = float4(1, 0, 0, 1);
            // q1
            else if (dx <0 && dy > 0)
                outputColor = float4(0, 1, 0, 1);
            // q2
            else if (dx > 0 && dy < 0)
                outputColor = float4(0, 0, 1, 1);
            // q3
            else if (dx < 0 && dy < 0)
                outputColor = float4(1, 1, 1, 1);
            else
                outputColor = float4(0, 0, 0, 1);
            break;
        }
        default: break;
    
    }
    return outputColor;
}