#include <Razix.h>

using namespace Razix;

class Sandbox : public Razix::RZApplication
{
public:
    Sandbox()
        : RZApplication(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Sandbox/")), "SponzaSandbox")
    //        : RZApplication(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Sandbox/")), "ShadowsSandbox")

    {
        Razix::RZInput::SelectGLFWInputManager();
        Razix::RZApplication::Get().Init();

        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
#ifdef RAZIX_PLATFORM_WINDOWS
        rzGfxCtx_SetRenderAPI(RZ_RENDER_API_D3D12);
#elif defined RAZIX_PLATFORM_MACOS
        rzGfxCtx_SetRenderAPI(RZ_RENDER_API_VULKAN);
#endif
        //-------------------------------------------------------------------------------------
    }

    void OnStart() override
    {
        // Add a default camera to the scene if it doesn't have one
        auto scene = RZSceneManager::Get().getCurrentScene();
        if (scene) {
            if (!scene->GetComponentsOfType<CameraComponent>().size()) {
                auto camera = scene->createEntity("PrimaryCamera");
                camera.AddComponent<CameraComponent>();
            }

            if (!scene->GetComponentsOfType<MeshRendererComponent>().size()) {
                // PBR materials test

                int   nrRows    = 7;
                int   nrColumns = 7;
                float spacing   = 2.5f;

                Razix::Gfx::MaterialProperties mat;
                mat.albedoColor      = glm::vec3(1.0f, 0.3f, 0.75f);
                mat.ambientOcclusion = 1.0f;

                for (int row = 0; row < nrRows; ++row) {
                    float metallic = (float) row / (float) nrRows;
                    for (int col = 0; col < nrColumns; ++col) {
                        float roughness                                             = glm::clamp((float) col / (float) nrColumns, 0.075f, 1.0f);
                        auto  pos                                                   = glm::vec3((col - (nrColumns / 2)) * spacing, (row - (nrRows / 2)) * spacing, 0.0f);
                        auto  sphereEntity                                          = RZSceneManager::Get().getCurrentScene()->createEntity("Sphere");
                        sphereEntity.GetComponent<TransformComponent>().Translation = pos;
                        sphereEntity.GetComponent<TransformComponent>().Scale       = glm::vec3(0.75f);
                        auto& mrc                                                   = sphereEntity.AddComponent<MeshRendererComponent>(Gfx::MeshPrimitive::Sphere);
                        auto  material                                              = mrc.Mesh->getMaterial();
                        mat.metallicColor                                           = metallic;
                        mat.roughnessColor                                          = roughness;
                        material->setProperties(mat);
                        // Save to file (assign a new and unique name)
                        auto name = "PBR_Test_Mat_M_" + std::to_string(metallic) + "_R_" + std::to_string(roughness);
                        material->setName(name);
                        material->saveToFile();
                    }
                }
            }

            if (!scene->GetComponentsOfType<LightComponent>().size()) {
                auto sun = scene->createEntity("Sun");
                sun.AddComponent<LightComponent>();
                auto& transform = sun.GetComponent<TransformComponent>();

                transform.Translation = {1.0f, 5.0f, 1.0f};
            }
        }
    }

    void OnRender() override
    {
    }
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new Sandbox();
}

RAZIX_PLATFORM_MAIN
