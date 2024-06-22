/**
 * ShaderLangCommon - provides common interface to support multiple shader languages cross-compilation from HLSL source
 */

#ifdef __GLSL__ // GLSL - shading language for Vulkan (SPIRV) & OpenGL.


// https://github.com/microsoft/DirectXShaderCompiler/wiki/Vulkan-combined-image-sampler-type

#elif defined __HLSL__ // HLSL - DirectX backend shading language.

#elif defined __PSSL__ // PSSL - PlayStation Shading Language for PlayStation consoles (PS4/PS5 etc.).

#elif defined __METAL__ // METAL - Metal shading language for Metal API for apple devices.

#endif