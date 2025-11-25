#pragma once

namespace Razix {

    struct RZWorld
    {
        u32                        worldInFlightIdx = 0;
        RZDynamicArray<rz_handle>  meshes;
        RZDynamicArray<rz_handle>  decals;
        RZSceneCamera              primaryCamera;
        LightsData                 lightsData;
        Gfx::rz_gfx_texture_handle sceneRenderTarget;
        Gfx::RZRendererSettings    rendererSettings;
    };

}    // namespace Razix