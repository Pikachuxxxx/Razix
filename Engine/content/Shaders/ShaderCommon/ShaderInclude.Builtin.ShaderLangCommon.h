/**
 * ShaderLangCommon - provides common interface to support multiple shader languages cross-compilation from HLSL source
 */

#ifdef __GLSL__    // GLSL - shading language for Vulkan (SPIRV) & OpenGL.

// https://github.com/microsoft/DirectXShaderCompiler/wiki/Vulkan-combined-image-sampler-type
// https://github.com/microsoft/DirectXShaderCompiler/blob/main/docs/SPIR-V.rst

    #define PUSH_CONSTANT(T) [[vk::push_constant]] T pcData
    #define GET_PUSH_CONSTANT(mem) pcData.mem

#elif defined __HLSL__    // HLSL - DirectX backend shading language.

    #define PUSH_CONSTANT(T)         \
        cbuffer T##Buffer : register(b1, space0) \
        {                                  \
            T pcData;                      \
        };
    #define GET_PUSH_CONSTANT(mem) pcData.mem

#elif defined __PSSL__    // PSSL - PlayStation Shading Language for PlayStation consoles (PS4/PS5 etc.).

    #define PUSH_CONSTANT(T)
    #define GET_PUSH_CONSTANT(mem) mem

#elif defined __METAL__    // METAL - Metal shading language for Metal API for apple devices.

    #define PUSH_CONSTANT(T)
    #define GET_PUSH_CONSTANT(mem) mem

#endif