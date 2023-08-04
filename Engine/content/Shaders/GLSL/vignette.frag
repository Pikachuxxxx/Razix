/*
 * Razix Engine Shader File
 * Used to render a screen space quad (White)
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable

 //------------------------------------------------------------------------------
 // Vertex Input
 layout(location = 0) in VSOutput
 {
     vec2 fragUV;
 }fs_in;

 layout (push_constant) uniform Resolution{
    vec2 resolution;
}resolutionData;
 //------------------------------------------------------------------------------
 // Fragment Shader Stage Uniforms
layout(set = 0, binding = 0) uniform sampler2D texSampler;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments 
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------
void main()
{

	vec2 uv = fs_in.fragUV;
   
    uv *=  1.0 - uv.yx;   //vec2(1.0)- uv.yx; -> 1.-u.yx; Thanks FabriceNeyret !
    
    float vig = uv.x*uv.y * 15.0; // multiply with sth for intensity
    
    vig = pow(vig, 0.25); // change pow for modifying the extend of the  vignette

    outFragColor = vec4(fs_in.fragUV, 0.0f, 1.0f);
    outFragColor = texture(texSampler, fs_in.fragUV) * vec4(vec3(vig), 1.0f);

}
//------------------------------------------------------------------------------
