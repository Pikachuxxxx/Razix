/*
 * Razix Engine Shader File
 * Fragment shader to render a skybox as a cubemap
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable
 //------------------------------------------------------------------------------
 // Bindless Textures
//#define ENABLE_BINDLESS 1
//#include <Common/ShaderInclude.Builtin.BindlessResources.glsl>
 //------------------------------------------------------------------------------
 // Vertex Input
 layout(location = 0) in VSOutput
 {
     vec3 fragLocalPos;
     float time;
 }fs_in;
 //------------------------------------------------------------------------------
 // Fragment Shader Stage Uniforms
layout(set = 1, binding = 0) uniform samplerCube environmentMap;
//layout (push_constant) uniform EnvMap
//{
//    uint idx;
//}tex;

//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
void main()
{
    //vec3 envColor = texture(global_textures_cubemap[nonuniformEXT(tex.idx)], fs_in.fragLocalPos).rgb;  
    vec3 envColor = texture(environmentMap, fs_in.fragLocalPos).rgb;  
    
    outSceneColor = vec4(envColor, 1.0);
}
//------------------------------------------------------------------------------
