#pragma once

#include "Razix/Gfx/GfxUtil.h"
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
        bool              EnableAPIValidation  = true;
        bool              EnableMSAA           = false;
        bool              EnableBindless       = false;
        bool              EnableBarrierLogging = false;
        GfxQualityMode    GfxQuality           = GfxQualityMode::kHigh;
        PerfMode          PerformanceMode      = PerfMode::kFidelity;
        rz_gfx_resolution PreferredResolution  = RZ_GFX_RESOLUTION_1440p;
        rz_gfx_target_fps TargetFPSCap         = RZ_GFX_TARGET_FPS_120;
        int               MaxShadowCascades    = 4;
        int               MSAASamples          = 4;
    };
}    // namespace Razix