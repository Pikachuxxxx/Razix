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
            
            // Let's create the assets first
            rz_asset_handle cube_transform_asset = RZAssetDB::Get().createAsset<RZTransformAsset>("Cube_Transform_Asset"); 
            rz_asset_handle cube_mesh_asset = RZAssetDB::Get().createAsset<RZMeshAsset>("Cube_Mesh_Asset");

            // Get the asset data and fill it with some data, for now we can just leave it empty or fill with some dummy data
            // RZTransformAsset* cube_transform_asset_data = RZAssetDB::Get().getAssetResourceMutablePtr()<RZTransformAsset>(cube_transform_asset);
            // we need to get a little weird here to fill the transform asset manaually since its SOA design, using scene manager makes it easu but for now deal with it.
            u32 default_transform_payload_idx = RAZIX_ASSET_HANDLE_GET_PAYLOAD_INDEX(cube_transform_asset);
            RZAssetDB::Get().GetAssetPoolRef<RZTransformAsset>().setPosition(default_transform_payload_idx, float4(0.0f, 0.0f, 0.0f, 1.0f));
            RZAssetDB::Get().GetAssetPoolRef<RZTransformAsset>().setRotation(default_transform_payload_idx, float4(1.0f, 1.0f, 1.0f, 1.0f));
            RZAssetDB::Get().GetAssetPoolRef<RZTransformAsset>().setScale(default_transform_payload_idx, float4(1.0f, 1.0f, 1.0f, 1.0f));

            RZMeshAsset* cube_mesh_asset_data = RZAssetDB::Get().getAssetResourceMutablePtr<RZMeshAsset>(cube_mesh_asset);
            RAZIX_UNUSED(cube_mesh_asset_data);

            i32 cube_transform_node_idx = rz_scene_graph_attach_asset(m_TanuDefaultSceneGraph, default_tanu_scn_zone_0->rootNodeIndex, "<rz_scn_node_cube_transform>", rz_uuid_generate(), cube_transform_asset);
            // Now add Mesh node as child to this first level parent node
            i32 cube_mesh_node_idx = rz_scene_graph_attach_asset(m_TanuDefaultSceneGraph, cube_transform_node_idx, "<rz_scn_node_cube_mesh>", rz_uuid_generate(), cube_mesh_asset);
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
    rz_scene_graph* m_TanuDefaultSceneGraph = NULL;
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Tanu Application");
    return new TanuGameApp();
}

RAZIX_PLATFORM_MAIN
