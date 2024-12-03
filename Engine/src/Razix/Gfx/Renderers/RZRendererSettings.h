#pragma once

namespace Razix {
    namespace Gfx {
#define NUM_HALTON_SAMPLES_TAA_JITTER 16

        // Renderer Settings + Debug flags
        enum RendererFeatures : u32
        {
            RendererFeature_None = 0,

            RendererFeature_Shadows    = 1 << 0,
            RendererFeature_GI         = 1 << 1,
            RendererFeature_IBL        = 1 << 2,
            RendererFeature_SSAO       = 1 << 3,
            RendererFeature_SSR        = 1 << 4,
            RendererFeature_Bloom      = 1 << 5,
            RendererFeature_FXAA       = 1 << 6,
            RendererFeature_TAA        = 1 << 7,
            RendererFeature_ImGui      = 1 << 8,
            RendererFeature_Deferred   = 1 << 9,
            RendererFeature_DebugDraws = 1 << 10,
            RendererFeature_CSM        = 1 << 11,
            RendererFeature_Skybox     = 1 << 12,
            RendererFeature_Tonemap    = 1 << 13,

            RendererFeature_Default = RendererFeature_Shadows | RendererFeature_ImGui | RendererFeature_IBL | RendererFeature_Deferred | RendererFeature_Skybox | RendererFeature_DebugDraws | RendererFeature_FXAA | RendererFeature_Tonemap,

            RendererFeature_All = RendererFeature_Default | RendererFeature_SSR,
        };

        enum RendererDebugFlags : u32
        {
            RendererDebugFlag_None            = 0,
            RendererDebugFlag_VisWireframe    = 1 << 0,
            RendererDebugFlag_VisSSAO         = 1 << 1,
            RendererDebugFlag_VisPreTonemap   = 1 << 2,
            RendererDebugFlag_VisQuadOverDraw = 1 << 3,
            RendererDebugFlag_VisUVs          = 1 << 4,
            RendererDebugFlag_VisAlbedo       = 1 << 5,
            RendererDebugFlag_VisCSMCascades  = 1 << 6,
        };

        enum Antialising
        {
            NoAA = 0,    // Render as-is
            FXAA,        // Fast sampled anti-aliasing
            TAA          // Temporal anti-aliasing
        };

        enum TonemapMode : u32
        {
            ACES,
            Filmic,
            Lottes,
            Reinhard,
            Reinhard_V2,
            Uchimura,
            Uncharted2,
            Unreal,
            None
        };

        enum SceneSamplingPattern : u32
        {
            Normal,
            Halton,
            Stratified
        };

        struct RZRendererSettings
        {
            u32 renderFeatures{RendererFeature_Default};
            struct
            {
                f32 radius{0.005f};
                f32 strength{0.04f};
            } bloomConfig;
            struct
            {
                int32_t numPropagations{6};
            } globalIlluminationConfig;
            u32                  debugFlags{0u};
            TonemapMode          tonemapMode         = ACES;
            Antialising          aaMode              = NoAA;
            SceneSamplingPattern samplingPattern     = Halton;
            bool                 useProceduralSkybox = false;
        };
    }    // namespace Gfx
}    // namespace Razix