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
layout(set = 1, binding = 0) uniform DirectionalLight
{
    vec3 position;
    vec3 color;
    vec3 viewPos;
} directional_light;
//------------------------------------------------------------------------------
// Materials and lightmaps
 layout(set = 2, binding = 0)  uniform MaterialProperties
 {
    vec4  albedoColor;
    vec4  roughnessColor;
    vec4  metallicColor;
    vec4  emissiveColor;
    bool  isUsingAlbedoMap;
    bool  isUsingNormalMap;
    bool  isUsingMetallicMap;
    bool  isUsingRoughnessMap;
    bool  isUsingAOMap;
    bool  isUsingEmissiveMap;
    int   workflow;
 }material;
// Having the samplers in the same set is adding to the Descriptor Block size which is fucked up
layout(set = 3, binding = 0) uniform sampler2D albedoMap;
layout(set = 3, binding = 1) uniform sampler2D normalMap;
layout(set = 3, binding = 2) uniform sampler2D metallicMap;
layout(set = 3, binding = 3) uniform sampler2D roughnessMap;
layout(set = 3, binding = 4) uniform sampler2D aoMap;
layout(set = 3, binding = 5) uniform sampler2D emissiveMap;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------

void main()
{
    // ambient
    vec3 ambient;// = directional_light.ambient * texture(albedoMap, fs_in.fragTexCoord).rgb;
    if(material.isUsingAlbedoMap){
       ambient = directional_light.color * texture(albedoMap, fs_in.fragTexCoord).rgb * material.albedoColor.rgb;
    }else {
        ambient = directional_light.color * material.albedoColor.rgb;
    }

    // diffuse
    vec3 norm;
    if(material.isUsingNormalMap){
        norm = texture(normalMap, fs_in.fragTexCoord).rgb;
    }else{
        norm  = normalize(fs_in.fragNormal);
    }
    vec3 lightDir = normalize(directional_light.position - fs_in.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse;
    if(material.isUsingAlbedoMap){
       diffuse = diff * texture(albedoMap, fs_in.fragTexCoord).rgb;
    }else {
        diffuse = diff * material.albedoColor.rgb;
    }

    // specular
    vec3 viewDir = normalize(directional_light.viewPos - fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    float spec_map = texture(metallicMap, fs_in.fragTexCoord).g;
    vec3 specular;// = vec3(spec_map, spec_map, spec_map) * spec;
     if(material.isUsingMetallicMap){
       specular = directional_light.color * spec * spec_map * material.metallicColor.rgb;
    }else {
        specular = directional_light.color * spec * material.metallicColor.rgb;
    }

    vec3 result = ambient;
    outFragColor = vec4(result, 1.0);

    //outFragColor = texture(albedoMap, fs_in.fragTexCoord);
}
//------------------------------------------------------------------------------
