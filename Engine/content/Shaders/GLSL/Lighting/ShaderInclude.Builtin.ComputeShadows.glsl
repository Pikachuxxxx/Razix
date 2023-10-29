#ifndef _SHADOW_CALC_GLSL_
#define _SHADOW_CALC_GLSL_
//------------------------------------------------------------------------------
// Simple Shadow Map calculation

// Shadow Filtering 
// [Source]: https://www.shadertoy.com/view/wtXXR8
#define RANDOM_ROTATION
#define ROT_BLEND 0.5 // rot ammount [0, 2] balance noise vs aliasing
#define MAX_TAPS 16
#define ITERATIONS 8.0
#define K_SIZE 0.5
#define CENTER_TAP // wether to use a tap at 0, 0

float hash12(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

const vec2 taps[MAX_TAPS] = vec2[MAX_TAPS](	
#ifdef CENTER_TAP
    vec2 ( 0., 0.),
#endif 
    vec2 ( 1., 0.),
    vec2 (-1.,-1.),
    vec2 ( 0., 1.),
    vec2 ( 0.,-2.), 
    vec2 (-1., 1.),
    vec2 ( 2., 3.),
    vec2 (-2., 0.),
    vec2 ( 0., 3.),
    vec2 ( 1.,-1.), 
    vec2 ( 3., 1.),
    vec2 (-3.,-3.),
    vec2 (-3., 2.),
    vec2 ( 4.,-1.),
    vec2 (-3.,-1.),
    vec2 ( 4.,-4.) 
#ifndef CENTER_TAP    
    ,vec2 (-4.,-2.)
#endif   
);

float DirectionalShadowCalculation(sampler2D shadowMap, vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    vec3 transformed_projCoords = projCoords * 0.5 + 0.5;
    vec2 uv = transformed_projCoords.xy;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, uv).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.00005); 
    //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
   
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
            shadow += currentDepth - bias > pcfDepth ? 0.01f : 1.0f;        
        }    
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}
//------------------------------------------------------------------------------
#endif