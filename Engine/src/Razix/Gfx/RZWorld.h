#ifndef RZ_WORLD_H
#define RZ_WORLD_H

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/RZHandle.h"

#include "Razix/Gfx/Renderers/RZRendererSettings.h"

// TODO: remove this dependency, just use rz_handle
#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/Cameras/RZCamera3D.h"

namespace Razix {

    class RZCamera3D;
    struct LightsData;

    struct RZWorld
    {
        u32                       worldInFlightIdx = 0;    // index of the current world buffer in flight, used for debugging only
        RZDynamicArray<rz_handle> meshes;
        RZDynamicArray<rz_handle> decals;
        RZDynamicArray<rz_handle> lights;
        RZCamera3D                primaryCamera;
        rz_gfx_texture_handle     sceneRenderTarget;
        Gfx::RZRendererSettings   rendererSettings;
    };

}    // namespace Razix
#endif    // RZ_WORLD_H
