#ifndef _COLOR_UTILS_GLSL_
#define _COLOR_UTILS_GLSL_

// [Source]: https://www.shadertoy.com/view/sdG3zz

/**
 * Returns a 3-component random color for a given integer hash value
 */
float3 RandomColorHash(uint p)
{
    p         = 1103515245U * ((p >> 1U) ^ p);
    uint  h32 = 1103515245U * (p ^ (p >> 3U));
    uint  n   = h32 ^ (h32 >> 16);
    uint3 rz  = uint3(n, n * 16807U, n * 48271U);
    return float3(
        float(rz.x & 0x7fffffffU) / 0x7fffffff,
        float(rz.y & 0x7fffffffU) / 0x7fffffff,
        float(rz.z & 0x7fffffffU) / 0x7fffffff);
}
//---------------------------------------------------------
// [Source]: https://gamedev.stackexchange.com/questions/92015/optimized-linear-to-srgb-glsl
// Converts a color from linear light gamma to sRGB gamma. Leaves the alpha channel alone, since it's already linear
// Linear [0,1] -> sRGB [0,1]
float3 LinearTosRGB(float3 linearRGB)
{
    // sRGB piecewise:
    // if x <= 0.0031308:  x * 12.92
    // else:               1.055 * x^(1/2.4) - 0.055

    const float3 cutoffVal = float3(0.0031308, 0.0031308, 0.0031308);
    const float3 a         = float3(1.055, 1.055, 1.055);
    const float3 b         = float3(0.055, 0.055, 0.055);
    const float3 linearMul = float3(12.92, 12.92, 12.92);
    const float3 gamma     = float3(1.0 / 2.4, 1.0 / 2.4, 1.0 / 2.4);

    // step(edge, x): 0 if x < edge, 1 otherwise
    float3 cutoff = step(cutoffVal, linearRGB);    // 0 for lower branch, 1 for higher

    float3 higher = a * pow(linearRGB, gamma) - b;    // gamma branch
    float3 lower  = linearRGB * linearMul;            // linear branch

    // lerp(x, y, s): x*(1-s) + y*s
    return lerp(lower, higher, cutoff);
}

// 4-component overload
float4 LinearTosRGB(float4 color)
{
    return float4(LinearTosRGB(color.rgb), color.a);
}

// sRGB [0,1] -> Linear [0,1]
float3 sRGBToLinear(float3 sRGB)
{
    // Inverse sRGB:
    // if x <= 0.04045:  x / 12.92
    // else:            ((x + 0.055) / 1.055) ^ 2.4

    const float3 cutoffVal = float3(0.04045, 0.04045, 0.04045);
    const float3 a         = float3(0.055, 0.055, 0.055);
    const float3 b         = float3(1.055, 1.055, 1.055);
    const float3 linearDiv = float3(12.92, 12.92, 12.92);
    const float3 gamma     = float3(2.4, 2.4, 2.4);

    float3 cutoff = step(cutoffVal, sRGB);    // 0 for lower branch, 1 for higher

    float3 higher = pow((sRGB + a) / b, gamma);    // gamma branch
    float3 lower  = sRGB / linearDiv;              // linear branch

    return lerp(lower, higher, cutoff);
}

// 4-component overload
float4 sRGBToLinear(float4 color)
{
    return float4(sRGBToLinear(color.rgb), color.a);
}

// Luminance (linear RGB, Rec.709)
float GetLuminance(float3 rgb)
{
    return dot(rgb, float3(0.2126, 0.7152, 0.0722));
}

//---------------------------------------------------------
#endif