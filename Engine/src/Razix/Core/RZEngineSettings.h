#pragma once

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {

    enum class PerfMode
    {
        kNone = 0,
        kFidelity,
        kPerformance,
        COUNT
    };

    enum class GfxQualityMode
    {
        kLow = 0,
        kMedium,
        kHigh,
        COUNT
    };

    // stored in .ini file
    struct EngineSettings
    {
        bool            EnableAPIValidation  = true;
        bool            EnableMSAA           = false;
        bool            EnableBindless       = false;
        bool            EnableBarrierLogging = false;
        GfxQualityMode  GfxQuality           = GfxQualityMode::kHigh;
        PerfMode        PerformanceMode      = PerfMode::kFidelity;
        Gfx::Resolution PreferredResolution  = Gfx::Resolution::k1440p;
        Gfx::TargetFPS  TargetFPSCap         = Gfx::TargetFPS::k120;
        int             MaxShadowCascades    = 4;
        int             MSAASamples          = 4;
    };

    /**
      * Graphics Features as supported by the GPU, even though Engine supports them
      * the GPU can override certain setting and query run-time info like LaneWidth etc.
      */
    typedef struct GraphicsFeatures
    {
        bool EnableVSync                  = false; /* No V-Sync by default we don't cap the frame rate */
        bool TesselateTerrain             = true;
        bool SupportsBindless             = true;
        bool SupportsWaveIntrinsics       = false;
        bool SupportsShaderModel6         = false;
        bool SupportsNullIndexDescriptors = false;
        u32  MaxBindlessTextures          = 4096;
        u32  MinLaneWidth                 = 0;
        u32  MaxLaneWidth                 = 0;
    } GraphicsFeatures;
}    // namespace Razix