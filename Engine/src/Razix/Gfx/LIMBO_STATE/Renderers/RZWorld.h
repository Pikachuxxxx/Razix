#pragma once

namespace Razix {

    class RZSceneView;

    struct RZWorld
    {
        std::vector<Gfx::RZMeshHandle>  meshes;
        std::vector<Gfx::RZDecalHandle> decals;
        std::vector<RZSceneView*>       sceneViews;
    };

}    // namespace Razix