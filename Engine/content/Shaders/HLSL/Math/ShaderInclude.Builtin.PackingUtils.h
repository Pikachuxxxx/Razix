#ifndef PACKING_UTILS_H
#define PACKING_UTILS_H
//------------------------------------------------------------------------

uint pack2Floats(float low, float high)
{
    return uint((f32tof16(low) & 0xFFFF) | ((f32tof16(high) & 0xFFFF) << 16));
}

float2 unpack2Floats(uint p)
{
    return float2(f16tof32(p & 0xFFFF), f16tof32((p >> 16) & 0xFFFF));
}

// Pack and Unpack 4x8 bits to/from float4 (Unorm4x8)

//------------------------------------------------------------------------
uint packSnorm2x16(float2 v)
{
    uint2 SNorm = uint2(round(clamp(v, -1, 1) * 32767.0));
    return (0x0000FFFF & SNorm.x) | ((SNorm.y << 16) & 0xFFFF0000);
}

float2 unpackSnorm2x16(uint p)
{
    half2 ret = half2(
        clamp((0x0000FFFF & p) / 32767.0, -1, 1),
        clamp(((0xFFFF0000 & p) >> 16) / 32767.0, -1, 1));

    return ret;
}
//------------------------------------------------------------------------

uint packUnorm2x16(float2 v)
{
    uint2 UNorm = uint2(round(saturate(v) * 65535.0));
    return (0x0000FFFF & UNorm.x) | ((UNorm.y << 16) & 0xFFFF0000);
}

float2 unpackUnorm2x16(uint p)
{
    float2 ret;
    ret.x = saturate((0x0000FFFF & p) / 65535.0);
    ret.y = saturate(((0xFFFF0000 & p) >> 16) / 65535.0);
    return ret;
}
//------------------------------------------------------------------------

uint packUnorm4x8(float4 v)
{
    uint4 UNorm = uint4(round(saturate(v) * 255.0));
    return (0x000000FF & UNorm.x) | ((UNorm.y << 8) & 0x0000FF00) | ((UNorm.z << 16) & 0x00FF0000) | ((UNorm.w << 24) & 0xFF000000);
}

float4 unpackUnorm4x8(uint p)
{
    return float4(float(p & 0x000000FF) / 255.0,
        float((p & 0x0000FF00) >> 8) / 255.0,
        float((p & 0x00FF0000) >> 16) / 255.0,
        float((p & 0xFF000000) >> 24) / 255.0);
}
//------------------------------------------------------------------------

uint64_t packUint2x32(uint2 value)
{
    return (uint64_t(value.y) << 32) | uint64_t(value.x);
}

uint2 unpackUint2x32(uint64_t value)
{
    uint2 Unpacked;
    Unpacked.x = uint(value & 0xffffffff);
    Unpacked.y = uint(value >> 32);
    return Unpacked;
}
//------------------------------------------------------------------------

#endif