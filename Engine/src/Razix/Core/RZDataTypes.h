#ifndef RZ_DATA_TYPES_H
#define RZ_DATA_TYPES_H

#if defined(__GLSL__) || defined(__HLSL__) || defined(__PSSL__) || defined(__MSL__) || defined(__SPIRV__) || defined(__LLVM__) || defined(__CUDA__)
    #define f32 float
    #define d32 double
    #define u32 uint
    #define i32 int
    #define u16 half
    #define i16 half
    #define u8  uchar
    #define i8  char
#else

    #include <stddef.h>    // UNIX
    #include <stdint.h>

    // remove using  for shader compatibility
    #include <glm/gtx/compatibility.hpp>
using namespace glm;

typedef vec<1, uint, defaultp> uint1;
typedef vec<2, uint, defaultp> uint2;
typedef vec<3, uint, defaultp> uint3;
typedef vec<4, uint, defaultp> uint4;

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
#endif

#endif    // RZ_DATA_TYPES_H
