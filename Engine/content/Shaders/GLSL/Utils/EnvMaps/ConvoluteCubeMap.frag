#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * COnvolutes a cubemap to calculate the total irradiance 
 */
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable

//------------------------------------------------------------------------------
// Constants
const float PI = 3.14159265359;
//------------------------------------------------------------------------------
// Vertex Input
 layout(location = 0) in VSOutput
 {
    vec3 normal;
    vec2 texCoord;
    vec3 localPos;
    int layer;
 }vs_in;
//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
layout (set = 0, binding = 1) uniform samplerCube envMap;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments 
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------
void main()
{
    vec3 localPos = vs_in.localPos;
    localPos.y = -localPos.y;
    vec3 normal = normalize(localPos);

    vec3 irradiance = vec3(0.0f);

    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, normal);
    up         = cross(normal, right);
    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
       for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
       {
           // spherical to cartesian (in tangent space)
           vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
           // tangent space to world
           vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
           irradiance += textureLod(envMap, sampleVec, 0).rgb * cos(theta) * sin(theta);
           nrSamples++;
       }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    outFragColor = vec4(irradiance, 1.0f);
}
