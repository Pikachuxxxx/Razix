/*
 * Razix Engine Shader File
 * Renders a mesh using phong shading using light maps (albedo, roughness, metallic, specular, normal etc.)
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable

 #include <Material/Material.glsl>
 #include <Lighting/Light.glsl>

 //------------------------------------------------------------------------------
 // Vertex Input
layout(location = 0) in VSOutput
{
    vec3 fragPos;
    vec4 fragColor;
    vec2 fragTexCoord;
    vec3 fragNormal;
    vec3 fragTangent;
}fs_in;
//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
DECLARE_LIGHT_BUFFER(2, 0, lightBuffer)
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------
// Functions

vec3 CalcDirLight(LightData light, vec3 normal)
{
    // Ambient
    vec3 ambient  = light.color.rgb  * vec3(texture(albedoMap, fs_in.fragTexCoord)) * 0.1f;

    // Diffuse
    vec3 lightDir = normalize(light.direction.xyz);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * vec3(texture(albedoMap, fs_in.fragTexCoord));
     
    // Specular shading
    vec3 viewDir = normalize(-fs_in.fragPos);
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * vec3(texture(metallicMap, fs_in.fragTexCoord));

    // combine results
    return ambient + diffuse + specular;
}  
//------------------------------------------------------------------------------
// Main
void main()
{
    vec4 normal = texture(normalMap, fs_in.fragTexCoord);
    vec3 result = CalcDirLight(lightBuffer.data[0], fs_in.fragNormal);
    outFragColor = vec4(result, 1.0);
}
//------------------------------------------------------------------------------
