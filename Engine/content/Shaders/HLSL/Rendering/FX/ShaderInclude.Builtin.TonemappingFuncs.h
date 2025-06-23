#ifndef _TONEMAPPING_H_
#define _TONEMAPPING_H_

namespace TonemapFuncs {

    ////////////////////////////////////////////////////////////////////////////////
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    float3 ACES(float3 x)
    {
        const float a = 2.51f;
        const float b = 0.03f;
        const float c = 2.43f;
        const float d = 0.59f;
        const float e = 0.14f;
        return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
    }

    // -----------------------------------------------------------------------------
    // Filmic Tonemapping Operators http://filmicworlds.com/blog/filmic-tonemapping-operators/
    float3 Filmic(float3 x)
    {
        float3 X      = max(0.0f, x - 0.004f);
        float3 result = (X * (6.2f * X + 0.5f)) / (X * (6.2f * X + 1.7f) + 0.06f);
        return pow(result, float3(2.2f, 2.2f, 2.2f));
    }

    // -----------------------------------------------------------------------------
    // Lottes 2016, "Advanced Techniques and Optimization of HDR Color Pipelines"
    float3 Lottes(float3 x)
    {
        const float3 a      = float3(1.6f, 1.6f, 1.6f);
        const float3 d      = float3(0.977f, 0.977f, 0.977f);
        const float3 hdrMax = float3(8.0f, 8.0f, 8.0f);
        const float3 midIn  = float3(0.18f, 0.18f, 0.18f);
        const float3 midOut = float3(0.267f, 0.267f, 0.267f);

        const float3 b =
            (-pow(midIn, a) + pow(hdrMax, a) * midOut) /
            ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
        const float3 c =
            (pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
            ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

        return pow(x, a) / (pow(x, a * d) * b + c);
    }

    // -----------------------------------------------------------------------------
    // Reinhard
    float3 Reinhard(float3 x)
    {
        return x / (1.0f + x);
    }

    // -----------------------------------------------------------------------------
    // Reinhard II (variant)
    float3 Reinhard2(float3 x)
    {
        const float L_white = 4.0f;

        return (x * (1.0f + x / (L_white * L_white))) / (1.0f + x);
    }

    // -----------------------------------------------------------------------------
    // Uchimura 2017, "HDR theory and practice"
    // Math: https://www.desmos.com/calculator/gslcdxvipg
    // Source: https://www.slideshare.net/nikuque/hdr-theory-and-practicce-jp
    float3 Uchimura(float3 x, float P, float a, float m, float l, float c, float b)
    {
        float l0 = ((P - m) * l) / a;
        float L0 = m - m / a;
        float L1 = m + (1.0f - m) / a;
        float S0 = m + l0;
        float S1 = m + a * l0;
        float C2 = (a * P) / (P - S1);
        float CP = -C2 / P;

        float3 w0 = float3(1.0f - smoothstep(0.0f, m, x.r),
            1.0f - smoothstep(0.0f, m, x.g),
            1.0f - smoothstep(0.0f, m, x.b));
        float3 w2 = float3(step(m + l0, x.r),
            step(m + l0, x.g),
            step(m + l0, x.b));
        float3 w1 = float3(1.0f - w0.r - w2.r,
            1.0f - w0.g - w2.g,
            1.0f - w0.b - w2.b);

        float3 T = float3(m * pow(x.r / m, c) + b,
            m * pow(x.g / m, c) + b,
            m * pow(x.b / m, c) + b);
        float3 S = float3(P - (P - S1) * exp(CP * (x.r - S0)),
            P - (P - S1) * exp(CP * (x.g - S0)),
            P - (P - S1) * exp(CP * (x.b - S0)));
        float3 L = float3(m + a * (x.r - m),
            m + a * (x.g - m),
            m + a * (x.b - m));

        return float3(T.r * w0.r + L.r * w1.r + S.r * w2.r,
            T.g * w0.g + L.g * w1.g + S.g * w2.g,
            T.b * w0.b + L.b * w1.b + S.b * w2.b);
    }

    float3 Uchimura(float3 x)
    {
        const float P = 1.0f;     // max display brightness
        const float a = 1.0f;     // contrast
        const float m = 0.22f;    // linear section start
        const float l = 0.4f;     // linear section length
        const float c = 1.33f;    // black
        const float b = 0.0f;     // pedestal

        return Uchimura(x, P, a, m, l, c, b);
    }

    float Uchimura(float x, float P, float a, float m, float l, float c, float b)
    {
        float l0 = ((P - m) * l) / a;
        float L0 = m - m / a;
        float L1 = m + (1.0f - m) / a;
        float S0 = m + l0;
        float S1 = m + a * l0;
        float C2 = (a * P) / (P - S1);
        float CP = -C2 / P;

        float w0 = 1.0f - smoothstep(0.0f, m, x);
        float w2 = step(m + l0, x);
        float w1 = 1.0f - w0 - w2;

        float T = m * pow(x / m, c) + b;
        float S = P - (P - S1) * exp(CP * (x - S0));
        float L = m + a * (x - m);

        return T * w0 + L * w1 + S * w2;
    }

    float Uchimura(float x)
    {
        const float P = 1.0f;     // max display brightness
        const float a = 1.0f;     // contrast
        const float m = 0.22f;    // linear section start
        const float l = 0.4f;     // linear section length
        const float c = 1.33f;    // black
        const float b = 0.0f;     // pedestal

        return Uchimura(x, P, a, m, l, c, b);
    }

    // -----------------------------------------------------------------------------
    // Uncharted 2
    float3 Uncharted2Tonemap(float3 x)
    {
        float A = 0.15f;
        float B = 0.50f;
        float C = 0.10f;
        float D = 0.20f;
        float E = 0.02f;
        float F = 0.30f;
        float W = 11.2f;
        return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
    }

    float3 Uncharted2(float3 color)
    {
        const float W            = 11.2f;
        float       exposureBias = 2.0f;
        float3      curr         = Uncharted2Tonemap(exposureBias * color);
        float3      whiteScale   = 1.0f / Uncharted2Tonemap(float3(W, W, W));
        return curr * whiteScale;
    }

    float Uncharted2Tonemap(float x)
    {
        float A = 0.15f;
        float B = 0.50f;
        float C = 0.10f;
        float D = 0.20f;
        float E = 0.02f;
        float F = 0.30f;
        float W = 11.2f;
        return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
    }

    float Uncharted2(float color)
    {
        const float W            = 11.2f;
        const float exposureBias = 2.0f;
        float       curr         = Uncharted2Tonemap(exposureBias * color);
        float       whiteScale   = 1.0f / Uncharted2Tonemap(W);
        return curr * whiteScale;
    }

    // -----------------------------------------------------------------------------
    // Unreal
    // Unreal 3, Documentation: "Color Grading"
    // Adapted to be close to Tonemap_ACES, with similar range
    // Gamma 2.2 correction is baked in, don't use with sRGB conversion!
    float3 Unreal(float3 x)
    {
        // Reverse gamma
        return pow(x / (x + 0.155f) * 1.019f, float3(2.2f, 2.2f, 2.2f));
    }

    float Unreal(float x)
    {
        // Reverse gamma
        return pow(x / (x + 0.155f) * 1.019f, 2.2f);
    }

    ////////////////////////////////////////////////////////////////////////////////
}    // namespace TonemapFuncs
#endif