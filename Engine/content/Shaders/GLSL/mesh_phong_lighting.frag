/*
 * Razix Engine Shader File
 * Renders a mesh using phong shading using light maps (albedo, roughness, metallic, specular, normal etc.)
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable

#include <Material/ShaderInclude.Builtin.Material.glsl>
#include <Lighting/ShaderInclude.Builtin.Light.glsl>
#include <FX/tonemapping.glsl>

 #define SHADOW_MAP_CASCADE_COUNT 4
 //------------------------------------------------------------------------------
 // Vertex Input
layout(location = 0) in VSOutput
{
    vec3 fragPos;
    vec4 fragColor;
    vec2 fragUV;
    vec3 fragNormal;
    vec3 fragTangent;
    vec3 viewPos;
}fs_in;
//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
DECLARE_LIGHT_BUFFER(2, 0, lightBuffer)
//layout(set = 3, binding = 0) uniform sampler2DArray CascadedShadowMaps;
layout(set = 3, binding = 0) uniform sampler2D shadowMap;
layout(set = 3, binding = 1) uniform ShadowMapData {
    //vec4 cascadeSplits;
	//mat4 cascadeViewProjMat[SHADOW_MAP_CASCADE_COUNT];
    mat4 lightSpaceMatrix;
}shadowMapData;
const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
// Functions
// Cascaded Shadow Map calculation
//float textureProj(vec4 shadowCoord, vec2 offset, uint cascadeIndex)
//{
//	float shadow = 1.0;
//	float bias = 0.005;
//
//	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) {
//		float dist = texture(CascadedShadowMaps, vec3(shadowCoord.st + offset, cascadeIndex)).r;
//		if (shadowCoord.w > 0 && dist < shadowCoord.z - bias) {
//			shadow = 0.3;
//		}
//	}
//	return shadow;
//
//}
//------------------------------------------------------------------------------
// Simple Shadow Map calculation
float DirectionalShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    vec3 transformed_projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, transformed_projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    float shadow = currentDepth - bias > closestDepth  ? 0.1 : 1.0;  

    // PCF
    //vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    //for(int x = -1; x <= 1; ++x)
    //{
    //    for(int y = -1; y <= 1; ++y)
    //    {
    //        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
    //        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.25;        
    //    }    
    //}
    //shadow /= 9.0;

    return shadow;
}
// Point Light Omnidirectional Shadow Map calculation
float OmnidirectionalShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    return 0.0f;
}
//------------------------------------------------------------------------------
// Directional light Calculation
vec3 CalculateDirectionalLightContribution(LightData light, vec3 normal, vec3 viewPos)
{
    // Ambient
    float ambientStrength  = 0.1f;
    vec3 ambient  = ambientStrength * light.color.rgb * Mat_getAlbedoColor(fs_in.fragUV);

    // Diffuse
    // Since IDK how to use the light direction I will use the position and normalize it
    vec3 lightDir = normalize(light.position.xyz);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * Mat_getAlbedoColor(fs_in.fragUV);
     
    // Specular shading
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * light.color * getSpecularColor(fs_in.fragUV);

    // combine results
    return ambient + diffuse + specular;
}  
//------------------------------------------------------------------------------
// Point Light Calculation
/**
 * Calculates the point light contribution 
 *
 * light The Light data
 * normal The normal vector of the surface
 * viewPos The vector from which the scene is viewed from
 */
vec3 CalculatePointLightContribution(LightData light, vec3 normal, vec3 viewPos)
{
    // Ambient
    float ambientStrength  = 0.1f;
    vec3 ambient  = ambientStrength * light.color.rgb * Mat_getAlbedoColor(fs_in.fragUV);

    // Diffuse
    vec3 lightDir = normalize(light.position - fs_in.fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * Mat_getAlbedoColor(fs_in.fragUV);
     
    // Specular shading
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * light.color * getSpecularColor(fs_in.fragUV);

    // attenuation
    float distance    = length(light.position - fs_in.fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient *= attenuation;  
    diffuse *= attenuation;
    specular *= attenuation;   

    // combine results
    return ambient + diffuse + specular;
}
//------------------------------------------------------------------------------
// Spot Light Calculation
vec3 CalculateSpotLightContribution(LightData light, vec3 normal, vec3 viewPos)
{
    return vec3(0.0f);
}
//------------------------------------------------------------------------------
// Main
void main()
{   
    vec3 normal = normalize(fs_in.fragNormal);
    outSceneColor = vec4(normal, 1.0f);
    return;
    // transform normal vector to range [-1,1]
    //normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    vec3 result = vec3(0.0f);
    if(lightBuffer.data[0].type == LightType_Directional)
        result += CalculateDirectionalLightContribution(lightBuffer.data[0], normalize(normal), fs_in.viewPos);
    else if(lightBuffer.data[0].type == LightType_Point)
        result += CalculatePointLightContribution(lightBuffer.data[0], normalize(normal), fs_in.viewPos);

    // Discard alpha samples
    if(Mat_getOpacity(fs_in.fragUV) < 0.1)
        discard;
    //-----------------------------------------------
    // Opacity check
    outSceneColor = vec4(result, Mat_getOpacity(fs_in.fragUV));

    //-----------------------------------------------
    // Shadow map calculation
    vec4 FragPosLightSpace = shadowMapData.lightSpaceMatrix * vec4(fs_in.fragPos, 1.0);
    float shadow = DirectionalShadowCalculation(FragPosLightSpace, normalize(normal), lightBuffer.data[0].position);
     
    //outSceneColor.rgb *= shadow;

    //-----------------------------------------------
    // Gamma correction
    //float gamma = 2.2;
    //outSceneColor.rgb = pow(outSceneColor.rgb, vec3(1.0/gamma));

    //-----------------------------------------------
    // Test the CSM at layer 0
    //float depthValue = texture(CascadedShadowMaps, vec3(fs_in.fragUV, 0)).r;
    //outSceneColor = vec4(vec3(depthValue), 1.0);
    //-----------------------------------------------
    // Cascaded Shadow calculation
    // Get cascade index for the current fragment's view position
    //uint cascadeIndex = 0;
    //for(uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; ++i) {
    //	if(fs_in.viewPos.z < shadowMapData.cascadeSplits[i]) {	
    //		cascadeIndex = i + 1;
    //	}
    //}
    // Depth compare for shadowing
    //vec4 shadowCoord = (biasMat * shadowMapData.cascadeViewProjMat[cascadeIndex]) * vec4(fs_in.fragPos, 1.0);	
    //float shadow = 0;
    //shadow = textureProj(shadowCoord / shadowCoord.w, vec2(0.0), cascadeIndex);
}
//------------------------------------------------------------------------------
