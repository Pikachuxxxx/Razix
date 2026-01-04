#pragma once

namespace Razix {

    class RZCamera3D;
    struct LightsData;
    namespace Gfx {
        struct RZRendererSettings;
        struct rz_gfx_texture_handle;
    }

    struct RZWorld
    {
        u32                        worldInFlightIdx = 0;
        RZDynamicArray<rz_handle>  meshes;
        RZDynamicArray<rz_handle>  decals;
        RZCamera3D                 primaryCamera;
        LightsData                 lightsData;
        Gfx::rz_gfx_texture_handle sceneRenderTarget;
        Gfx::RZRendererSettings    rendererSettings;
    };

}    // namespace Razix