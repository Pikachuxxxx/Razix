#ifndef _GAUSSIAN_TAP_FILTERS_GLSL_
#define _GAUSSIAN_TAP_FILTERS_GLSL_

// [Source]: https://github.com/Jam3/glsl-fast-gaussian-blur
// [Source]: https://www.rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/

// Usage : Blurs the image from the specified uv coordinate, using the given resolution (size in pixels of screen) and direction -- typically either [1, 0] (horizontal) or [0, 1] (vertical).

const uint FiveTap = 0;
const uint NineTap = 1; 
const uint ThirteenTap = 2; 

// 5 tap filter
vec4 blur5(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
    vec4 color = vec4(0.0);
    const vec2 off1 = vec2(1.3333333333333333) * direction;
    color += texture(image, uv) * 0.29411764705882354;
    color += texture(image, uv + (off1 / resolution)) * 0.35294117647058826;
    color += texture(image, uv - (off1 / resolution)) * 0.35294117647058826;
    return color;
}

// 9 tap filter
vec4 blur9(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
    vec4 color = vec4(0.0);
    const vec2 off1 = vec2(1.3846153846) * direction;
    const vec2 off2 = vec2(3.2307692308) * direction;
    color += texture(image, uv) * 0.2270270270;
    color += texture(image, uv + (off1 / resolution)) * 0.3162162162;
    color += texture(image, uv - (off1 / resolution)) * 0.3162162162;
    color += texture(image, uv + (off2 / resolution)) * 0.0702702703;
    color += texture(image, uv - (off2 / resolution)) * 0.0702702703;
    return color;
}

// 13 tap filter
vec4 blur13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
    vec4 color = vec4(0.0);
    const vec2 off1 = vec2(1.411764705882353) * direction;
    const vec2 off2 = vec2(3.2941176470588234) * direction;
    const vec2 off3 = vec2(5.176470588235294) * direction;
    color += texture(image, uv) * 0.1964825501511404;
    color += texture(image, uv + (off1 / resolution)) * 0.2969069646728344;
    color += texture(image, uv - (off1 / resolution)) * 0.2969069646728344;
    color += texture(image, uv + (off2 / resolution)) * 0.09447039785044732;
    color += texture(image, uv - (off2 / resolution)) * 0.09447039785044732;
    color += texture(image, uv + (off3 / resolution)) * 0.010381362401148057;
    color += texture(image, uv - (off3 / resolution)) * 0.010381362401148057;
    return color;
}

#endif