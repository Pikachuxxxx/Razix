/**
 * ShaderLangCommon - provides common interface to support multiple shader languages cross-compilation from HLSL source
 */
// https://anteru.net/blog/2016/mapping-between-HLSL-and-GLSL/

// https://github.com/KhronosGroup/glslang/issues/1188
// Ex. Usage
//[[vk::builtin("PointSize")]]
//float PointSize : PSIZE;

#ifdef __GLSL__    // GLSL - shading language for Vulkan (SPIRV) & OpenGL.

// https://github.com/microsoft/DirectXShaderCompiler/wiki/Vulkan-combined-image-sampler-type
// https://github.com/microsoft/DirectXShaderCompiler/blob/main/docs/SPIR-V.rst

    #define PUSH_CONSTANT(T)       [[vk::push_constant]] T pcData
    #define GET_PUSH_CONSTANT(mem) pcData.mem

    #define ATTRIB_EARLY_DEPTH [earlydepthstencil]

#elif defined __HLSL__    // HLSL - DirectX backend shading language.

    #define PUSH_CONSTANT(T) \
        cbuffer T##Buffer    \
        {                    \
            T pcData;        \
        };
    #define GET_PUSH_CONSTANT(mem) pcData.mem

    #define ATTRIB_EARLY_DEPTH [earlydepthstencil]

#elif defined __PSSL__    // PSSL - PlayStation Shading Language for PlayStation consoles (PS4/PS5 etc.).

    #define PUSH_CONSTANT(T)       // same as HLSL ConstantBuffer<T>##Buffer, reflection will make us bind this every frame
    #define GET_PUSH_CONSTANT(mem) mem

    #define ATTRIB_EARLY_DEPTH

#elif defined __METAL__    // METAL - Metal shading language for Metal API for apple devices.

    #define PUSH_CONSTANT(T)
    #define GET_PUSH_CONSTANT(mem) mem

    #define ATTRIB_EARLY_DEPTH

#endif