#ifndef _SHADOW_CALC_GLSL_
#define _SHADOW_CALC_GLSL_
//------------------------------------------------------------------------------
// Simple Shadow Map calculation
// https://github.com/opengl-tutorials/ogl/blob/master/tutorial16_shadowmaps/ShadowMapping.fragmentshader

const mat4 kBiasMatrix = mat4(
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0
);

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

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
    //float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005); 
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    
    //float shadow = currentDepth - bias > closestDepth  ? 0.1 : 1.0;  
    
    // PCF - percentage closer filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            // TODO: https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-17-efficient-soft-edged-shadows-using
            // [Source] : https://gamedev.net/forums/topic/498755-randomly-rotated-pcf-shadows/4253985/
            vec2 jitterFactor = fract( uv.xy * vec2( 18428.4f, 23614.3f)) * 2.0f - 1.0f;
            
            float pcfDepth = texture(shadowMap, uv + jitterFactor * texelSize).r; 
            //float pcfDepth = texture(shadowMap, uv+ vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 0.1f : 1.0f;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
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
#endif