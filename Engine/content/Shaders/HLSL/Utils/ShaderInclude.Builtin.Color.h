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
/*
/ Converts a color from linear light gamma to sRGB gamma. Leaves the alpha channel alone, since it's already linear
float3 LinearTosRGB(float3 linearRGB)
{
    float3 cutoff = lessThan(linearRGB.rgb, float3(0.0031308, 0.0031308, 0.0031308));
    float3 higher = float3(1.055, 1.055, 1.055)*pow(linearRGB.rgb, float3(1.0/2.4, 1.0/2.4,1.0/2.4)) - float3(0.055, 0.055, 0.055);
    float3 lower = linearRGB.rgb * float3(12.92);

    return mix(higher, lower, cutoff);
}

// Helper function to convert a 4-component color from Linear to sRGB color space
float4 LinearTosRGB(float4 color) 
{ 
    return float4(LinearTosRGB(color.rgb), color.a); 
}

// Converts a color from sRGB gamma to linear light gamma
float3 sRGBToLinear(float3 sRGB)
{
    float3 cutoff = lessThan(sRGB.rgb, float3(0.04045));
    float3 higher = pow((sRGB.rgb + float3(0.055))/float3(1.055), float3(2.4));
    float3 lower = sRGB.rgb/float3(12.92);

    return mix(higher, lower, cutoff);
}

// Helper function to convert from sRGB back to linear for a 4-component color 
float4 sRGBToLinear(float4 color) 
{
    return float4(sRGBToLinear(color.rgb), color.a); 
}

// Gets the luminance value of the given color, useful for FXAA
float getLuminance(float3 rgb) 
{
    return dot(rgb, float3(0.2126, 0.7152, 0.0722)); 
}
*/
//---------------------------------------------------------
#endif