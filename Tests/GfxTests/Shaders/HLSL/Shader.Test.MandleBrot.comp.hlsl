/*
 * Razix Engine HLSL Fragment Shader File
 * Testing Compute shader in VK/DX12 using mandlebrot set generation
 * [Source]: https://www.shadertoy.com/view/NtKBWz
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
RWTexture2D<float4> g_OutputTexture : register(u0, space0);
//cbuffer Params : register(b1, space0)
//{
//    uint2 resolution;
//    int maxIter;
//}
//------------------------------------------------------------------------------
float4 GenerateMandelbrot(float2 uv) 
{
    float zx = 0.0;
    float zy = 0.0;

    int iteration = 0;
    int max_iteration = 256;


    // f(z) = z^2 + c, z and c are complex number
    [loop]
    while ((zx*zx + zy*zy) <= 256.0 && iteration < max_iteration)
    {
        float next_zx = zx*zx - zy*zy + uv.x;
        zy = 2.0*zx*zy + uv.y;
        
        zx = next_zx;
        
        iteration++;
    }

    float4 fragColor;
    if (iteration == max_iteration)
        fragColor = float4(0.0, 0.0, 0.0, 1.0);
    else
    {
        float smooth_iteration = float(iteration);
        
        // Smooth iteration use https://iquilezles.org/articles/msetsmooth/
        // |Zn| = zx*zx + zy*zy, B = 256.0, d = 2.0
        //smooth_iteration -= log(log(zx*zx + zy*zy)/log(256.0))/log(2.0);
        smooth_iteration -= log2(log2(zx*zx + zy*zy)) - 3.0; // Simplified with log2.
        
        fragColor = float4(smooth_iteration/25.0, (smooth_iteration-25.0)/25.0, (smooth_iteration-50.0)/25.0, 1.0);
    }
    return fragColor;
}

[numthreads(16, 16, 1)]
void CS_MAIN(uint3 DTid: SV_DispatchThreadID)
{
    float2 textureDim;
    g_OutputTexture.GetDimensions(textureDim.x, textureDim.y);
    float2 texelSize = 1.0f / textureDim;
    float2 uv = DTid.xy * texelSize;
    uv = uv * 4.0f - float2(2.5f, 2.0f);
    float4 color = GenerateMandelbrot(uv);
    g_OutputTexture[DTid.xy] = color;
}