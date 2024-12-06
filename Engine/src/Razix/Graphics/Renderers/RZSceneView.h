#pragma once

#include "Razix/Graphics/Lighting/LightData.h"
#include "Razix/Graphics/Renderers/RZRendererSettings.h"
#include "Razix/Graphics/SkyInfo.h"

#include "Razix/Scene/RZSceneCamera.h"

namespace Razix {

    struct RZSceneView
    {
        RZSceneCamera                primaryCamera;
        LightsData                   lightsData;
        Graphics::RZTextureHandle    sceneRenderTarget;
        Graphics::SkyInfo            skyInfo;
        Graphics::RZRendererSettings rendererSettings;
    };
}    // namespace Razix