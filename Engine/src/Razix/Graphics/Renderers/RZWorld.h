#pragma once

namespace Razix {

    class RZSceneView;

    struct RZWorld
    {
        std::vector<Graphics::RZMeshHandle>  meshes;
        std::vector<Graphics::RZDecalHandle> decals;
        std::vector<RZSceneView*>            sceneViews;
    };

}    // namespace Razix