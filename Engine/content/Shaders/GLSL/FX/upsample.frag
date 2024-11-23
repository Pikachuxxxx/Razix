/*
 * Razix Engine Shader File
 * Default Fragment Shader that can be used for rendering basic geometry with vertex colors and use a texture as well
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable

// This shader performs downsampling on a texture,
// as taken from Call Of Duty method, presented at ACM Siggraph 2014.
// This particular method was customly designed to eliminate
// "pulsating artifacts and temporal stability issues".

// Remember to add bilinear minification filter for this texture!
// Remember to use a floating-point texture format (for HDR)!
// Remember to use edge clamping for this texture!

 //------------------------------------------------------------------------------
 // Vertex Input
 layout(location = 0) in VSOutput
 {
     vec2 fragUV;
 }fs_in;

//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
layout(set = 0, binding = 0) uniform sampler2D srcTexture;
//------------------------------------------------------------------------------
layout (push_constant) uniform UpsamplePushConstantData{
    float filterRadius;
}pcData;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------
void main()
{
    vec3 upsample;
    // The filter kernel is applied with a radius, specified in texture
    // coordinates, so that the radius will vary across mip resolutions.
    float x = pcData.filterRadius;
    float y = pcData.filterRadius;

    // Take 9 samples around current texel:
    // a - b - c
    // d - e - f
    // g - h - i
    // === ('e' is the current texel) ===
    vec3 a = texture(srcTexture, vec2(fs_in.fragUV.x - x, fs_in.fragUV.y + y)).rgb;
    vec3 b = texture(srcTexture, vec2(fs_in.fragUV.x,     fs_in.fragUV.y + y)).rgb;
    vec3 c = texture(srcTexture, vec2(fs_in.fragUV.x + x, fs_in.fragUV.y + y)).rgb;

    vec3 d = texture(srcTexture, vec2(fs_in.fragUV.x - x, fs_in.fragUV.y)).rgb;
    vec3 e = texture(srcTexture, vec2(fs_in.fragUV.x,     fs_in.fragUV.y)).rgb;
    vec3 f = texture(srcTexture, vec2(fs_in.fragUV.x + x, fs_in.fragUV.y)).rgb;

    vec3 g = texture(srcTexture, vec2(fs_in.fragUV.x - x, fs_in.fragUV.y - y)).rgb;
    vec3 h = texture(srcTexture, vec2(fs_in.fragUV.x,     fs_in.fragUV.y - y)).rgb;
    vec3 i = texture(srcTexture, vec2(fs_in.fragUV.x + x, fs_in.fragUV.y - y)).rgb;

    // Apply weighted distribution, by using a 3x3 tent filter:
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    upsample = e*4.0;
    upsample += (b+d+f+h)*2.0;
    upsample += (a+c+g+i);
    upsample *= 1.0 / 16.0;

    outFragColor = vec4(upsample, 1.0f);
}