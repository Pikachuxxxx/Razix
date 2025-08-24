#pragma once

#define RZ_ENABLE_GFX 0

namespace Razix {
    namespace Gfx {

#define NUM_HALTON_SAMPLES_TAA_JITTER 16

        // TODO: Use bit structs instead?

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

        enum Antialising
        {
            NoAA = 0,
            FXAA,
            TAA
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
            Halton,    // Preferred for TAA
            Stratified
        };

        struct RendererDebugFlags
        {
            u32 None : 1;
            u32 VisWireframe : 1;
            u32 VisSSAO : 1;
            u32 VisPreTonemap : 1;
            u32 VisQuadOverDraw : 1;
            u32 VisUVs : 1;
            u32 VisAlbedo : 1;
            u32 VisCSMCascades : 1;
        };

        struct RZRendererSettings
        {
            u32                  renderFeatures      = RendererFeature_Default;
            RendererDebugFlags   debugFlags          = {0};
            TonemapMode          tonemapMode         = ACES;
            Antialising          aaMode              = NoAA;
            SceneSamplingPattern samplingPattern     = Halton;
            bool                 useProceduralSkybox = false;
        };
    }    // namespace Gfx
}    // namespace Razix