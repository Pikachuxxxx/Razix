#if 0
    #pragma once
    #include <Razix.h>

    #define GLM_FORCE_DEPTH_ZERO_TO_ONE

class RendererTest : public Razix::RZApplication
{
public:
    RendererTest()
        : RZApplication(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Sandbox/")), "FrameGraphTest")
    {
        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
        Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);
        //-------------------------------------------------------------------------------------

        Razix::RZInput::SelectGLFWInputManager();
        Razix::RZApplication::Get().Init();

        // Init Graphics Context
        //-------------------------------------------------------------------------------------
        // Creating the Graphics Context and Initialize it
        RAZIX_CORE_INFO("Creating Graphics Context...");
        Razix::Graphics::RZGraphicsContext::Create(RZApplication::Get().getWindowProps(), RZApplication::Get().getWindow());
        RAZIX_CORE_INFO("Initializing Graphics Context...");
        Razix::Graphics::RZGraphicsContext::GetContext()->Init();
        //-------------------------------------------------------------------------------------
    }

    ~RendererTest() {}

    void OnStart() override
    {
        // Add some model entities
    #if 0
        auto& modelEnitties = RZSceneManager::Get().getCurrentScene()->GetComponentsOfType<Razix::Graphics::RZModel>();
        if (!modelEnitties.size()) {
            // since Avocado is already there we load Sponza
            auto& spoznaModelEntity = RZSceneManager::Get().getCurrentScene()->createEntity("Sponza");
            spoznaModelEntity.AddComponent<Razix::Graphics::RZModel>("//Meshes/bistro.obj");
        }
    #endif

        // Add a directional light for test
        auto lightEnitties = RZSceneManager::Get().getCurrentScene()->GetComponentsOfType<Razix::LightComponent>();
        if (!lightEnitties.size()) {
            auto directionalLightEntity = RZSceneManager::Get().getCurrentScene()->createEntity("Directional Light");
            directionalLightEntity.AddComponent<Razix::LightComponent>();
            directionalLightEntity.GetComponent<Razix::LightComponent>().light.setDirection(glm::vec3(1.0f));
        }

        auto scripts = RZSceneManager::Get().getCurrentScene()->GetComponentsOfType<Razix::LuaScriptComponent>();
        if (!scripts.size()) {
            Razix::RZEntity imguiEntity = RZSceneManager::Get().getCurrentScene()->createEntity("imgui_script_entity");
            imguiEntity.AddComponent<Razix::LuaScriptComponent>();
            if (imguiEntity.HasComponent<Razix::LuaScriptComponent>()) {
                Razix::LuaScriptComponent& lsc = imguiEntity.GetComponent<Razix::LuaScriptComponent>();
                lsc.loadScript("//Scripts/imgui_test.lua");
            }
        }

        // Camera Entity
        auto cameras = RZSceneManager::Get().getCurrentScene()->GetComponentsOfType<Razix::CameraComponent>();
        if (!cameras.size()) {
            Razix::RZEntity camera = RZSceneManager::Get().getCurrentScene()->createEntity("Camera");
            camera.AddComponent<Razix::CameraComponent>();
            if (camera.HasComponent<Razix::CameraComponent>()) {
                Razix::CameraComponent& cc = camera.GetComponent<Razix::CameraComponent>();
                cc.Camera.setViewportSize(getWindow()->getWidth(), getWindow()->getHeight());
            }
        }
    }

    void OnResize(uint32_t width, uint32_t height) override
    {
    }
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new RendererTest();
}

int main(int argc, char** argv)
{
    EngineMain(argc, argv);

    while (Razix::RZApplication::Get().RenderFrame()) {}

    Razix::RZApplication::Get().Quit();
    Razix::RZApplication::Get().SaveApp();

    EngineExit();

    return EXIT_SUCCESS;
}
#endif