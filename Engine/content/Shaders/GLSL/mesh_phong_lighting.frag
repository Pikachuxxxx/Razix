/*
 * Razix Engine Shader File
 * Renders a mesh using phong shading using light maps (albedo, roughness, metallic, specular, normal etc.)
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
    vec3 fragPos;
    vec4 fragColor;
    vec2 fragTexCoord;
    vec3 fragNormal;
    vec3 fragTangent;
}fs_in;
 //------------------------------------------------------------------------------
 // Fragment Shader Stage Uniforms
 // Uniforms and Push Constants
// Lighting information
struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float time;
};
struct PointLight
{
    vec3  position;
    vec3  color;
    float radius;
    float constant;
    float linear;
    float quadratic;
};
struct SpotLight
{
    vec3  position;
    vec3  direction;
    vec3  color;
    float radius;
    float constant;
    float linear;
    float quadratic;
};
struct LightData
{
    int                 type;
    DirectionalLight    dirLightData;
    PointLight          pointLightData;
    SpotLight           spotLightData;
};
// Forward Light Data
 layout(set = 1, binding = 0) uniform ForwardLightData
{
    vec3        position;
    vec3        viewPos;
    LightData   lightData;
}forward_light_data;
//------------------------------------------------------------------------------
// Materials and lightmaps
 layout(set = 2, binding = 0) uniform PhongMaterialProperties
 {
    vec4 ambient;
    vec4 diffuse;
    float shininess;
 }material;
// Having the samplers in the same set is adding to the Descriptor Block size which is fucked up
layout(set = 3, binding = 0) uniform sampler2D aoMap;
layout(set = 3, binding = 1) uniform sampler2D diffuseMap;
layout(set = 3, binding = 2) uniform sampler2D normalMap;
layout(set = 3, binding = 3) uniform sampler2D specularMap;
// TODO: Add booleans to check if the sampler is available or not
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------
// Functions

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.color  * vec3(texture(diffuseMap, fs_in.fragTexCoord));
    vec3 diffuse  = light.color  * diff * vec3(texture(diffuseMap, fs_in.fragTexCoord));
    vec3 specular = light.color * spec * vec3(texture(specularMap, fs_in.fragTexCoord));
    return (ambient + diffuse + specular);
}  
//------------------------------------------------------------------------------
// Main
void main()
{
    vec3 result = CalcDirLight(forward_light_data.lightData.dirLightData, fs_in.fragNormal, forward_light_data.viewPos);
    outFragColor = vec4(result, 1.0);
    outFragColor = texture(diffuseMap, fs_in.fragTexCoord);
}
//------------------------------------------------------------------------------
