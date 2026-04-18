#include <Razix.h>

using namespace Razix;

class TanuGameApp : public Razix::RZApplication
{
public:
    TanuGameApp()
        : RZApplication(RZString(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + RZString("/Tanu/")), "Tanu")
    {
        RZApplication::Init();
    }

    void OnStart() override
    {
        // create a default scene just in case to test the rz_scene_graph API
        // RZApplication::Begin would've loaded the scene into memory from project files
        // We check the rz_scene_manager from engine for active or queued scenes and create one here programatically
        // assume on first loadup tanu has no scenes in project file until we habe blender export utils
        rz_scene_graph_manager& sceneGraphManager = RZEngine::Get().getSceneManager();
        if (sceneGraphManager.pActiveScene == NULL && m_TanuDefaultSceneGraph == NULL) {
            m_TanuDefaultSceneGraph = rz_scene_graph_create("<rz_scene_tanu_default_scn>", RZEngine::Get().getSystemAllocator(), RAZIX_SCENE_DIMS);
            sceneGraphManager.pActiveScene = m_TanuDefaultSceneGraph;

            // Add some test nodes to the scene graph, perhaps an empty cube
            rz_zone* default_tanu_scn_zone_0 = rz_scene_graph_get_active_zone(m_TanuDefaultSceneGraph);
            // Create a root level transform node under the zone root.
            i32 cube_transform_node_idx = rz_scene_graph_create_node(m_TanuDefaultSceneGraph, default_tanu_scn_zone_0->rootNodeIndex, "<rz_scn_node_cube_transform>", rz_uuid_generate(), RZAssetType::kTransform);
            // Now add Mesh node as child to this first level parent node
            i32 cube_mesh_node_idx = rz_scene_graph_create_node(m_TanuDefaultSceneGraph, cube_transform_node_idx, "<rz_scn_node_cube_mesh>", rz_uuid_generate(), RZAssetType::kMesh);
        }
    }

    void OnRender() override
    {
    }

    void OnQuit() override
    {
        rz_scene_graph_manager& sceneGraphManager = RZEngine::Get().getSceneManager();
        if (sceneGraphManager.pActiveScene != NULL) {
            rz_scene_graph_destroy(RZEngine::Get().getSystemAllocator(), m_TanuDefaultSceneGraph);
        }
    }

private:
    rz_scene_graph* m_TanuDefaultSceneGraph;
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Tanu Application");
    return new TanuGameApp();
}

RAZIX_PLATFORM_MAIN
