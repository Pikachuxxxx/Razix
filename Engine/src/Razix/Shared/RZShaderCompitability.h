#ifndef RZ_SHADER_COMPITABILITY_H
#define RZ_SHADER_COMPITABILITY_H

// If using this file in shaders...
#if defined(__GLSL__) || defined(__HLSL__) || defined(__PSSL__) || defined(__MSL__) || defined(__SPIRV__) || defined(__LLVM__) || defined(__CUDA__)

    #define fmax max

    // some common include files, that are valid to be included in shaders
    // default includes to make your life easy
    #include <Razix/Core/RZDataTypes.h>
    #include <Razix/Math/ImportanceSampling.h>
    #include <Razix/Math/MathConstants.h>

#else
    // If C/C++ code...
#endif    // shaders
#endif    // RZ_SHADER_COMPITABILITY_H
