// clang-format off
#include "rzxpch.h"
// clang-format on
#include "House.h"

namespace Razix {
    namespace Gfx {
        namespace HouseMD {

            RZWorld BuildRazixWorldFromSceneData(const rz_scene_graph* sceneGraph)
            {
                // TODO: parse scene graph for meshes (transforms have already been updated when we called rz_scene_graph_update)
                // Use the gather list in scene graph to get list of all active meshes in the scene scene graph

                RZWorld world;
                world.worldInFlightIdx = -1;    // will be filled by scene graph before hand-off so we don't care about it yet

                return world;
            }
        }    // namespace HouseMD
    }    // namespace Gfx
}    // namespace Razix
