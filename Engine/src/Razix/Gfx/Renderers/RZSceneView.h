#pragma once

#include "Razix/Gfx/Lighting/LightData.h"
#include "Razix/Gfx/Renderers/RZRendererSettings.h"

#include "Razix/Scene/RZSceneCamera.h"

namespace Razix {

    struct RZSceneView
    {
        RZSceneCamera           primaryCamera;
        LightsData              lightsData;
        Gfx::RZTextureHandle    sceneRenderTarget;
        Gfx::RZRendererSettings rendererSettings;
    };
}    // namespace Razix