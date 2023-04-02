/*
 * Razix Engine Shader File
 * Mixes the bloom and scene HDR texture
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable

 #include <FX/tonemapping.glsl>

 //------------------------------------------------------------------------------
 // Vertex Input
 layout(location = 0) in VSOutput
 {
     vec2 fragTexCoord;
 }fs_in;

 //------------------------------------------------------------------------------
 // Fragment Shader Stage Uniforms
layout(set = 0, binding = 0) uniform sampler2D sceneHDRTexture;
layout(set = 0, binding = 1) uniform sampler2D bloomBlurTexture;
//------------------------------------------------------------------------------
layout (push_constant) uniform BloomMixPushConstantData{
    float bloomStrength;
    uint toneMapMode;
}pcData;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------

void main()
{
    vec3 hdrColor = texture(sceneHDRTexture, fs_in.fragTexCoord).rgb;
    vec3 bloomColor = texture(bloomBlurTexture, fs_in.fragTexCoord).rgb;
    vec3 color =  mix(hdrColor, bloomColor, pcData.bloomStrength); // linear interpolation

    switch(pcData.toneMapMode)
    {
        case 0:
            color = ACES(color);
            break;
        case 1:
            color = Filmic(color);
            break;
        case 2:
            color = lottes(color);
            break;
        case 3:
            color = reinhard(color);
            break;
        case 4:
            color = reinhard2(color);
            break;
        case 5:
            color = uchimura(color);
            break;
        case 6:
            color = uncharted2(color);
            break;
        case 7:
            color = unreal(color);
            break;
        case 8:
            color = color;
            break;
    }

    // also gamma correct while we're at it
    //const float gamma = 2.2;
    //color = pow(color, vec3(1.0 / gamma));
    outFragColor = vec4(color, 1.0);
}
//------------------------------------------------------------------------------
