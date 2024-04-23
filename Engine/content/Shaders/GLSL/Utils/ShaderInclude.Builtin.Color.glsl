#ifndef _COLOR_UTILS_GLSL_
#define _COLOR_UTILS_GLSL_

// [Source]: https://www.shadertoy.com/view/sdG3zz
vec3 RandomColorHash(uint p)
{
    p = 1103515245U*((p >> 1U)^(p));
    uint h32 = 1103515245U*((p)^(p>>3U));
    uint n = h32^(h32 >> 16);
    uvec3 rz = uvec3(n, n*16807U, n*48271U);
    return vec3(rz & uvec3(0x7fffffffU))/float(0x7fffffff);
}

const float kGamma = 2.4;
const float kInvGamma = 1.0 / kGamma;

// [Source]: https://gamedev.stackexchange.com/questions/92015/optimized-linear-to-srgb-glsl

// Converts a color from linear light gamma to sRGB gamma. Leaves the alpha channel alone, since it's already linear.
vec3 LinearTosRGB(vec3 linearRGB)
{
    bvec3 cutoff = lessThan(linearRGB.rgb, vec3(0.0031308));
    vec3 higher = vec3(1.055)*pow(linearRGB.rgb, vec3(1.0/2.4)) - vec3(0.055);
    vec3 lower = linearRGB.rgb * vec3(12.92);

    return mix(higher, lower, cutoff);
}
vec4 LinearTosRGB(vec4 color) { return vec4(LinearTosRGB(color.rgb), color.a); }

// Converts a color from sRGB gamma to linear light gamma
vec3 sRGBToLinear(vec3 sRGB)
{
    bvec3 cutoff = lessThan(sRGB.rgb, vec3(0.04045));
    vec3 higher = pow((sRGB.rgb + vec3(0.055))/vec3(1.055), vec3(2.4));
    vec3 lower = sRGB.rgb/vec3(12.92);

    return mix(higher, lower, cutoff);
}
vec4 sRGBToLinear(vec4 color) { return vec4(sRGBToLinear(color.rgb), color.a); }

float getLuminance(vec3 rgb) { return dot(rgb, vec3(0.2126, 0.7152, 0.0722)); }

#endif