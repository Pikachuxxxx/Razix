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
}    // namespace Razix