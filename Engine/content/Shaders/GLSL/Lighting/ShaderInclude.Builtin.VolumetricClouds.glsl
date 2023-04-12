#ifndef _VOLUMETRIC_CLOUDS_GLSL_
#define _VOLUMETRIC_CLOUDS_GLSL_
/*
Source : https://www.shadertoy.com/view/MstBWs
Real time PBR Volumetric Clouds by robobo1221.
Single scattering
Also includes volumetric light.
http://shadertoy.com/user/robobo1221

First ever somewhat PBR effect I decided to work on.
It uses the same algorithm to calculate the worldPosition as in: https://www.shadertoy.com/view/lstfR7

Feel free to fork and edit it. (Credit me please.)
Hope you enjoy!
*/

#define VOLUMETRIC_LIGHT
//#define SPHERICAL_PROJECTION

#define cloudSpeed 0.02
#define cloudHeight 2000.0
#define cloudThickness 100.0
#define cloudDensity 0.1

#define fogDensity 0.003

#define volumetricCloudSteps 4			//Higher is a better result with rendering of clouds.
#define volumetricLightSteps 20			//Higher is a better result with rendering of volumetric light.

#define cloudShadowingSteps 12			//Higher is a better result with shading on clouds.
#define volumetricLightShadowSteps 4	//Higher is a better result with shading on volumetric light from clouds

#define rayleighCoeff (vec3(0.27, 0.5, 1.0) * 1e-5)	//Not really correct
#define mieCoeff vec3(0.5e-6)						//Not really correct

const float sunBrightness = 3.0;

#define earthRadius 6371000.0

//////////////////////////////////////////////////////////////////
layout(set = 1, binding = 0) uniform sampler2D NoiseTexture;
//------------------------------------------------------------------------------

float bayer2(vec2 a){
    a = floor(a);
    return fract( dot(a, vec2(.5, a.y * .75)) );
}

vec2 rsi(vec3 position, vec3 direction, float radius) {
    float PoD = dot(position, direction);
    float radiusSquared = radius * radius;

    float delta = PoD * PoD + radiusSquared - dot(position, position);
    if (delta < 0.0) return vec2(-1.0);
          delta = sqrt(delta);

    return -PoD + vec2(-delta, delta);
}

#define bayer4(a)   (bayer2( .5*(a))*.25+bayer2(a))
#define bayer8(a)   (bayer4( .5*(a))*.25+bayer2(a))
#define bayer16(a)  (bayer8( .5*(a))*.25+bayer2(a))
#define bayer32(a)  (bayer16(.5*(a))*.25+bayer2(a))
#define bayer64(a)  (bayer32(.5*(a))*.25+bayer2(a))
#define bayer128(a) (bayer64(.5*(a))*.25+bayer2(a))

//////////////////////////////////////////////////////////////////

#define cloudMinHeight cloudHeight
#define cloudMaxHeight (cloudThickness + cloudMinHeight)

#define sunPosition vec3(1.0, 1.0, 0.0)

const float pi = acos(-1.0);
const float rPi = 1.0 / pi;
const float hPi = pi * 0.5;
const float tau = pi * 2.0;
const float rLOG2 = 1.0 / log(2.0);

vec3 sphereToCart(vec3 sphere) {
    vec2 c = cos(sphere.xy);
    vec2 s = sin(sphere.xy);
    
    return sphere.z * vec3(c.x * c.y, s.y, s.x * c.y);
}

///////////////////////////////////////////////////////////////////////////////////

#define d0(x) (abs(x) + 1e-8)
#define d02(x) (abs(x) + 1e-3)

const vec3 totalCoeff = rayleighCoeff + mieCoeff;

vec3 scatter(vec3 coeff, float depth){
	return coeff * depth;
}

vec3 absorb(vec3 coeff, float depth){
	return exp2(scatter(coeff, -depth));
}

float calcParticleThickness(float depth){
   	
    depth = depth * 2.0;
    depth = max(depth + 0.01, 0.01);
    depth = 1.0 / depth;
    
	return 100000.0 * depth;   
}

float calcParticleThicknessH(float depth){
   	
    depth = depth * 2.0 + 0.1;
    depth = max(depth + 0.01, 0.01);
    depth = 1.0 / depth;
    
	return 100000.0 * depth;   
}

float calcParticleThicknessConst(const float depth){
    
	return 100000.0 / max(depth * 2.0 - 0.01, 0.01);   
}

float rayleighPhase(float x){
	return 0.375 * (1.0 + x*x);
}

float hgPhase(float x, float g)
{
    float g2 = g*g;
	return 0.25 * ((1.0 - g2) * pow(1.0 + g2 - 2.0*g*x, -1.5));
}

float miePhaseSky(float x, float depth)
{
 	return hgPhase(x, exp2(-0.000003 * depth));
}

float powder(float od)
{
	return 1.0 - exp2(-od * 2.0);
}

float calculateScatterIntergral(float opticalDepth, float coeff){
    float a = -coeff * rLOG2;
    float b = -1.0 / coeff;
    float c =  1.0 / coeff;

    return exp2(a * opticalDepth) * b + c;
}

vec3 calculateScatterIntergral(float opticalDepth, vec3 coeff){
    vec3 a = -coeff * rLOG2;
    vec3 b = -1.0 / coeff;
    vec3 c =  1.0 / coeff;

    return exp2(a * opticalDepth) * b + c;
}

vec3 calcAtmosphericScatter(vec3 sunDirection, vec3 worldViewDirection, out vec3 absorbLight)
{
    const float ln2 = log(2.0);
    
    float lDotW = dot(sunDirection, worldViewDirection);
    float lDotU = dot(sunDirection, vec3(0.0, 1.0, 0.0));
    float uDotW = dot(vec3(0.0, 1.0, 0.0), worldViewDirection);
    
	float opticalDepth = calcParticleThickness(uDotW);
    float opticalDepthLight = calcParticleThickness(lDotU);
    
    vec3 scatterView = scatter(totalCoeff, opticalDepth);
    vec3 absorbView = absorb(totalCoeff, opticalDepth);
    
    vec3 scatterLight = scatter(totalCoeff, opticalDepthLight);
         absorbLight = absorb(totalCoeff, opticalDepthLight);
    	 
    vec3 absorbSun = abs(absorbLight - absorbView) / d0((scatterLight - scatterView) * ln2);
    
    vec3 mieScatter = scatter(mieCoeff, opticalDepth) * miePhaseSky(lDotW, opticalDepth);
    vec3 rayleighScatter = scatter(rayleighCoeff, opticalDepth) * rayleighPhase(lDotW);
    
    vec3 scatterSun = mieScatter + rayleighScatter;
    
    vec3 sunSpot = smoothstep(0.9999, 0.99993, lDotW) * absorbView * sunBrightness;
    
    return (scatterView);
}

vec3 calcAtmosphericScatterTop(vec3 sunDirection){
    const float ln2 = log(2.0);
    
    float lDotU = dot(sunDirection, vec3(0.0, 1.0, 0.0));
    
	float opticalDepth = calcParticleThicknessConst(1.0);
    float opticalDepthLight = calcParticleThickness(lDotU);
    
    vec3 scatterView = scatter(totalCoeff, opticalDepth);
    vec3 absorbView = absorb(totalCoeff, opticalDepth);
    
    vec3 scatterLight = scatter(totalCoeff, opticalDepthLight);
    vec3 absorbLight = absorb(totalCoeff, opticalDepthLight);
    
    vec3 absorbSun = d02(absorbLight - absorbView) / d02((scatterLight - scatterView) * ln2);
    
    vec3 mieScatter = scatter(mieCoeff, opticalDepth) * 0.25;
    vec3 rayleighScatter = scatter(rayleighCoeff, opticalDepth) * 0.375;
    
    vec3 scatterSun = mieScatter + rayleighScatter;
    
    return (scatterSun * absorbSun) * sunBrightness;
}

float Get3DNoise(vec3 pos) 
{
    float p = floor(pos.z);
    float f = pos.z - p;
    
    const float invNoiseRes = 1.0 / 128.0;
    
    float zStretch = 25.0 * invNoiseRes;
    
    vec2 coord = pos.xy * invNoiseRes + (p * zStretch);
    
    vec2 noise = vec2(texture(NoiseTexture, coord).x, texture(NoiseTexture, coord + zStretch).x);
    
    return mix(noise.x, noise.y, f);
}

float getClouds(vec3 p, float time)
{
    p = vec3(p.x, length(p + vec3(0.0, earthRadius, 0.0)) - earthRadius, p.z);
    
    if (p.y < cloudMinHeight || p.y > cloudMaxHeight)
        return 0.0;

    vec3 movement = vec3(time, 0.0, time) * cloudSpeed;
    
    vec3 cloudCoord = (p * 0.001) + movement;
    
	float noise = Get3DNoise(cloudCoord) * 0.5;
    	  noise += Get3DNoise(cloudCoord * 2.0 + movement) * 0.25;
    	  noise += Get3DNoise(cloudCoord * 7.0 - movement) * 0.125;
    	  noise += Get3DNoise((cloudCoord + movement) * 16.0) * 0.0625;
    
    const float top = 0.004;
    const float bottom = 0.02;
    
    float horizonHeight = p.y - cloudMinHeight;
    float treshHold = (1.0 - exp2(-bottom * horizonHeight)) * exp2(-top * horizonHeight);
    
    float clouds = smoothstep(0.55, 0.6, noise);
          clouds *= treshHold;
    
    return clouds * cloudDensity;
}
    
float getCloudShadow(vec3 p, vec3 sunDirection)
{
	const int steps = volumetricLightShadowSteps;
    float rSteps = cloudThickness / float(steps) / abs(sunDirection.y);
    
    vec3 increment = sunDirection* rSteps;
    vec3 position = sunDirection* (cloudMinHeight - p.y) / sunDirection.y + p;
    
    float transmittance = 0.0;
    
    for (int i = 0; i < steps; i++, position += increment)
    {
		transmittance += getClouds(position, 1.0f);
    }
    
    return exp2(-transmittance * rSteps);
}

float getSunVisibility(vec3 p, vec3 sunDirection, float time)
{
	const int steps = cloudShadowingSteps;
    const float rSteps = cloudThickness / float(steps);
    
    vec3 increment = sunDirection * rSteps;
    vec3 position = increment * 0.5 + p;
    
    float transmittance = 0.0;
    
    for (int i = 0; i < steps; i++, position += increment)
    {
		transmittance += getClouds(position, time);
    }
    
    return exp2(-transmittance * rSteps);
}

float phase2Lobes(float x)
{
    const float m = 0.6;
    const float gm = 0.8;
    
	float lobe1 = hgPhase(x, 0.8 * gm);
    float lobe2 = hgPhase(x, -0.5 * gm);
    
    return mix(lobe2, lobe1, m);
}

vec3 getVolumetricCloudsScattering(float opticalDepth, float phase, vec3 p, vec3 sunColor, vec3 skyLight, vec3 sunDirection, float time)
{
    float intergal = calculateScatterIntergral(opticalDepth, 1.11);
    
    float beersPowder = powder(opticalDepth * log(2.0));
    
	vec3 sunlighting = (sunColor * getSunVisibility(p, sunDirection, time) * beersPowder) * phase * hPi * sunBrightness;
    vec3 skylighting = skyLight * 0.25 * rPi;
    
    return (sunlighting + skylighting) * intergal * pi;
}

float getHeightFogOD(float height)
{
	const float falloff = 0.001;
    
    return exp2(-height * falloff) * fogDensity;
}

vec3 getVolumetricLightScattering(float opticalDepth, float phase, vec3 p, vec3 sunColor, vec3 skyLight, vec3 sunDirection)
{
    float intergal = calculateScatterIntergral(opticalDepth, 1.11);
    
	vec3 sunlighting = sunColor * phase * hPi * sunBrightness;
         sunlighting *= getCloudShadow(p, sunDirection);
    vec3 skylighting = skyLight * 0.25 * rPi;
    
    return (sunlighting + skylighting) * intergal * pi;
}

vec3 calculateVolumetricClouds(vec3 sunDirection, vec3 worldViewDirection, vec3 color, float dither, vec3 sunColor, float time)
{
	const int steps = volumetricCloudSteps;
    const float iSteps = 1.0 / float(steps);
    
    float bottomSphere = rsi(vec3(0.0, 1.0, 0.0) * earthRadius, worldViewDirection, earthRadius + cloudMinHeight).y;
    float topSphere = rsi(vec3(0.0, 1.0, 0.0) * earthRadius, worldViewDirection, earthRadius + cloudMaxHeight).y;
    
    vec3 startPosition = worldViewDirection * bottomSphere;
    vec3 endPosition = worldViewDirection * topSphere;
    
    vec3 increment = (endPosition - startPosition) * iSteps;
    vec3 cloudPosition = increment * dither + startPosition;
    
    float stepLength = length(increment);
    
    vec3 scattering = vec3(0.0);
    float transmittance = 1.0;
    
    float lDotW = dot(sunDirection, worldViewDirection);
    float phase = phase2Lobes(lDotW);
    
    vec3 skyLight = calcAtmosphericScatterTop(sunDirection);

    for (int i = 0; i < steps; i++, cloudPosition += increment)
    {
        float opticalDepth = getClouds(cloudPosition, time) * stepLength;
        
        if (opticalDepth <= 0.0)
            continue;
        
		scattering += getVolumetricCloudsScattering(opticalDepth, phase, cloudPosition, sunColor, skyLight, sunDirection, time) * transmittance;
        transmittance *= exp2(-opticalDepth);
    }
    return mix(color * transmittance + scattering, color, clamp(length(startPosition) * 0.00001, 0.0, 1.0));
}

vec3 robobo1221Tonemap(vec3 color)
{
    #define rTOperator(x) (x / sqrt(x*x+1.0))

    float l = length(color);

    color = mix(color, color * 0.5, l / (l+1.0));
    color = rTOperator(color);

    return color;
}
#endif