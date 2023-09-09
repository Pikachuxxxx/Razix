#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Vertex shader to convert equirectangular hdr file to cubemap, uses a Cube mesh in vertex shader to render stuff onto
 */
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
// This extension is for #include support in glsl, this extension is usually not supported to force enable it perhaps?
#extension GL_ARB_shader_viewport_layer_array : enable
 //------------------------------------------------------------------------------
 //#define ENABLE_BINDLESS 1
 //#include <Common/ShaderInclude.Builtin.BindlessResources.glsl>
 //------------------------------------------------------------------------------
 // Vertex Input
 layout(location = 0) in VSOutput
 {
    vec3 normal;
    vec2 texCoord;
    vec3 localPos;
    int layer;
 }fs_in;
//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
//layout (push_constant) uniform EquirectnagularMapIdx
//{
//    uint idx;
//}tex;
 layout (set = 0, binding = 1) uniform sampler2D equirectangularMap;
 
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments 
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
//------------------------------------------------------------------------------
void main()
{
    vec2 uv = SampleSphericalMap(normalize(fs_in.localPos));
    //vec3 color = texture(global_textures_2d[nonuniformEXT(tex.idx)], uv).rgb;
    vec3 color = texture(equirectangularMap, uv).rgb;

    outFragColor = vec4(color, 1.0f);
}
