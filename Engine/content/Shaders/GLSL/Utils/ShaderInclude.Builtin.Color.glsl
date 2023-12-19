#ifndef _COLOR_UTILS_GLSL_
#define _COLOR_UTILS_GLSL_

vec3 RandomColorHash(uint p)
{
    p = 1103515245U*((p >> 1U)^(p));
    uint h32 = 1103515245U*((p)^(p>>3U));
    uint n = h32^(h32 >> 16);
    uvec3 rz = uvec3(n, n*16807U, n*48271U);
    return vec3(rz & uvec3(0x7fffffffU))/float(0x7fffffff);
}

#endif