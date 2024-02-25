#ifndef _SHADOW_CALC_GLSL_
#define _SHADOW_CALC_GLSL_
//------------------------------------------------------------------------------
// Simple Shadow Map calculation
// MAYBE TRY THIS LATER: https://github.com/opengl-tutorials/ogl/blob/master/tutorial16_shadowmaps/ShadowMapping.fragmentshader

#define SHADOW_MAP_CASCADE_COUNT 4

const mat4 kBiasMatrix = mat4(
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0
);

float textureProj(sampler2DArray shadowMap, vec4 shadowCoord, vec2 offset, uint cascadeIndex)
{
	float shadow = 1.0;
	float bias = 0.005;

	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) {
		float dist = texture(shadowMap, vec3(shadowCoord.st + offset, cascadeIndex)).r;
		if (shadowCoord.w > 0 && dist < shadowCoord.z - bias) {
			shadow = 0.1;
		}
	}
	return shadow;
}

float filterPCF(sampler2DArray shadowMap,vec4 sc, uint cascadeIndex)
{
	ivec2 texDim = textureSize(shadowMap, 0).xy;
	float scale = 0.75;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++) {
		for (int y = -range; y <= range; y++) {
			shadowFactor += textureProj(shadowMap, sc, vec2(dx*x, dy*y), cascadeIndex);
			count++;
		}
	}
	return shadowFactor / count;
}


// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float DirectionalShadowCalculation(sampler2D shadowMap, vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{

#if 1
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    vec3 transformed_projCoords = projCoords * 0.5 + 0.5;
    vec2 uv = transformed_projCoords.xy;
    vec3 uvw = transformed_projCoords;
    
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, uv).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    //float bias = max(0.0001 * (1.0 - dot(normal, lightDir)), 0.0001); 
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);  
    
    // 1 represents it's in shadow

    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  
    
    // PCF - percentage closer filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            // TODO: https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-17-efficient-soft-edged-shadows-using
            // [Source] : https://gamedev.net/forums/topic/498755-randomly-rotated-pcf-shadows/4253985/
            //vec2 jitterFactor = fract( uv.xy * vec2( 18428.4f, 23614.3f)) * 2.0f - 1.0f;
            
            //float pcfDepth = texture(shadowMap, uv + jitterFactor * texelSize).r; 
            float pcfDepth = texture(shadowMap, uv + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
      shadow = 0.0;
    
    // light contribution is 0 when it's in shadow
    return (1.0f - shadow);
#endif


#if 0
    vec4 ShadowCoord = fragPosLightSpace;

    float cosTheta = clamp(dot(normal, lightDir), 0.0f, 1.0f);
    float visibility=1.0;
    
	// Fixed bias, or...
	//float bias = 0.5;
    
	// ...variable bias
	float bias = 0.005*tan(acos(cosTheta));
	bias = clamp(bias, 0,0.01);
    
    for (int i=0;i<4;i++){
		// use either :
		//  - Always the same samples.
		//    Gives a fixed pattern in the shadow, but no noise
		//int index = i;
		//  - A random sample, based on the pixel's screen location. 
		//    No banding, but the shadow moves with the camera, which looks weird.
		// int index = int(16.0*random(gl_FragCoord.xyy, i))%16;
		//  - A random sample, based on the pixel's position in world space.
		//    The position is rounded to the millimeter to avoid too much aliasing
		int index = int(16.0*random(floor(fragPosLightSpace.xyz*1000.0), i))%16;
		
		// being fully in the shadow will eat up 4*0.2 = 0.8
		// 0.2 potentially remain, which is quite dark.
		visibility -= 0.2*(1.0-texture( shadowMap, vec3(ShadowCoord.xy + poissonDisk[index]/100.0,  (ShadowCoord.z-bias)/ShadowCoord.w) ));
	}
    return visibility;
#endif
}
//------------------------------------------------------------------------------
// CSM calculation
float DirectionalCSMShadowCalculation(sampler2DArray shadowMap, uint cascadeIdx, vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, float splitDist, float dt, float biasScale, float maxBias)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    vec3 transformed_projCoords = projCoords * 0.5 + 0.5;
    vec2 uv = transformed_projCoords.xy;
    
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, vec3(uv, cascadeIdx)).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(biasScale * (1.0 - dot(normal, lightDir)), maxBias); 
    //if (cascadeIdx == uint(SHADOW_MAP_CASCADE_COUNT - 1))
    //{
    //    bias *= 1 / (100.0f * 0.5f);
    //}
    //else
    //{
    //    bias *= 1 / (splitDist * 0.5f);
    //}
    // 1 represents it's in shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    //if(projCoords.z > 1.0)
    //  shadow = 0.0;

    // PCF - percentage closer filtering
    float shadow = 0.0;
    vec3 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            // TODO: https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-17-efficient-soft-edged-shadows-using
            // [Source] : https://gamedev.net/forums/topic/498755-randomly-rotated-pcf-shadows/4253985/
            vec2 jitterFactor = fract( uv.xy * vec2( 18428.4f * dt, 23614.3f * dt)) * 2.0f - 1.0f;
            
            //float pcfDepth = texture(shadowMap, uv + jitterFactor * texelSize).r; 
            float pcfDepth = texture(shadowMap, vec3(uv + vec2(x, y) * texelSize.xy, cascadeIdx)).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.25f;        
        }    
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
    {
        shadow = 0.0;
    }
    
    // light contribution is 0 when it's in shadow
    return (1.0f - shadow);
}
#endif