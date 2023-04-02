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
     vec2 fragTexCoord;
 }fs_in;

//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
layout(set = 0, binding = 0) uniform sampler2D srcTexture;
//------------------------------------------------------------------------------
layout (push_constant) uniform DownsamplePushConstantData{
    vec2 srcResolution;
}pcData;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------
void main()
{
    vec3 downsample;

    vec2 srcTexelSize = 1.0 / pcData.srcResolution;
    float x = srcTexelSize.x;
    float y = srcTexelSize.y;

    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ('e' is the current texel) ===
    vec3 a = texture(srcTexture, vec2(fs_in.fragTexCoord.x - 2*x, fs_in.fragTexCoord.y + 2*y)).rgb;
    vec3 b = texture(srcTexture, vec2(fs_in.fragTexCoord.x,       fs_in.fragTexCoord.y + 2*y)).rgb;
    vec3 c = texture(srcTexture, vec2(fs_in.fragTexCoord.x + 2*x, fs_in.fragTexCoord.y + 2*y)).rgb;

    vec3 d = texture(srcTexture, vec2(fs_in.fragTexCoord.x - 2*x, fs_in.fragTexCoord.y)).rgb;
    vec3 e = texture(srcTexture, vec2(fs_in.fragTexCoord.x,       fs_in.fragTexCoord.y)).rgb;
    vec3 f = texture(srcTexture, vec2(fs_in.fragTexCoord.x + 2*x, fs_in.fragTexCoord.y)).rgb;

    vec3 g = texture(srcTexture, vec2(fs_in.fragTexCoord.x - 2*x, fs_in.fragTexCoord.y - 2*y)).rgb;
    vec3 h = texture(srcTexture, vec2(fs_in.fragTexCoord.x,       fs_in.fragTexCoord.y - 2*y)).rgb;
    vec3 i = texture(srcTexture, vec2(fs_in.fragTexCoord.x + 2*x, fs_in.fragTexCoord.y - 2*y)).rgb;

    vec3 j = texture(srcTexture, vec2(fs_in.fragTexCoord.x - x, fs_in.fragTexCoord.y + y)).rgb;
    vec3 k = texture(srcTexture, vec2(fs_in.fragTexCoord.x + x, fs_in.fragTexCoord.y + y)).rgb;
    vec3 l = texture(srcTexture, vec2(fs_in.fragTexCoord.x - x, fs_in.fragTexCoord.y - y)).rgb;
    vec3 m = texture(srcTexture, vec2(fs_in.fragTexCoord.x + x, fs_in.fragTexCoord.y - y)).rgb;

    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap,
    // so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    // contribute 0.5 to the final color output. The code below is written
    // to effectively yield this sum. We get:
    // 0.125*5 + 0.03125*4 + 0.0625*4 = 1
    downsample = e*0.125;
    downsample += (a+c+g+i)*0.03125;
    downsample += (b+d+f+h)*0.0625;
    downsample += (j+k+l+m)*0.125;

    outFragColor = vec4(downsample, 1.0f);
}