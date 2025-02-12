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
        Razix::Gfx::RZGraphicsContext::SetRenderAPI(Razix::Gfx::RenderAPI::VULKAN);
#elif defined RAZIX_PLATFORM_MACOS
        Razix::Gfx::RZGraphicsContext::SetRenderAPI(Razix::Gfx::RenderAPI::VULKAN);
#endif
        //-------------------------------------------------------------------------------------

        // Init Graphics Context
        //-------------------------------------------------------------------------------------
        // Creating the Graphics Context and Initialize it
        RAZIX_CORE_INFO("Creating Graphics Context...");
        Razix::Gfx::RZGraphicsContext::Create(RZApplication::Get().getWindowProps(), RZApplication::Get().getWindow());
        RAZIX_CORE_INFO("Initializing Graphics Context...");
        Razix::Gfx::RZGraphicsContext::GetContext()->Init();
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
                // Add 5 spheres
                float spacing = 0.25;
                for (uint32_t i = 0; i < 5; i++) {
                    auto sphereMesh = scene->createEntity("Sphere_" + std::to_string(i));
                    sphereMesh.AddComponent<MeshRendererComponent>(Gfx::MeshPrimitive::Sphere);
                    auto& transform = sphereMesh.GetComponent<TransformComponent>();
                    transform.Translation.z += spacing * i;
                    transform.Scale *= 0.25f;
                }
            }

            if (!scene->GetComponentsOfType<LightComponent>().size()) {
                auto sun = scene->createEntity("Sun");
                sun.AddComponent<LightComponent>();
                auto& transform = sun.GetComponent<TransformComponent>();

                transform.Translation = {1.0f, 1.0f, 1.0f};
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
