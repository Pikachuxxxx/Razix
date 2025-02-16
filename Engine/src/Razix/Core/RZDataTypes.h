#ifndef RZ_DATA_TYPES_H
#define RZ_DATA_TYPES_H

// If using this file in shaders...
#if defined(__GLSL__) || defined(__HLSL__) || defined (__PSSL__) || defined(__MSL__) || defined (__SPIRV__) || defined(__LLVM__) || defined (__CUDA__)

    // careful with glm vec types, changing the math library will break the entire engine
    #define vec2   float2
    #define vec3   float3
    #define vec4   float4
    #define ivec2  uint2
    #define ivec3  uint3
    #define ivec4  uint4
    #define mat3   float3x3
    #define mat4   float4x4
    #define f32    float
    #define d32    double
    #define u32    uint
    #define i32    int
    #define u16    half
    #define i16    half
    #define u8     uchar
    #define i8     char

#else

    #include <stddef.h>    // UNIX
    #include <stdint.h>
    // Native types typedefs /////////////////////////////////////////////////
    typedef int8_t  i8;
    typedef int16_t i16;
    typedef int32_t i32;
    typedef int64_t i64;

    typedef uint8_t  u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef unsigned long ulong;

    typedef size_t sz;
    typedef float  f32;
    typedef double d32;

    typedef char        ch;
    typedef const char* cstr;

    //-------------------------------
    // Native type limits
    static const u64 u64_max = UINT64_MAX;
    static const i64 i64_max = INT64_MAX;
    static const u32 u32_max = UINT32_MAX;
    static const i32 i32_max = INT32_MAX;
    static const u16 u16_max = UINT16_MAX;
    static const i16 i16_max = INT16_MAX;
    static const u8  u8_max  = UINT8_MAX;
    static const i8  i8_max  = INT8_MAX;
    
    // remove using glm:: for shader compatibility
#include <glm/glm.hpp>
using namespace glm;
#endif // shaders
#endif // RZ_DATA_TYPES_H
