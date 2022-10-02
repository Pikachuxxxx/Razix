#pragma once
#include <Razix.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

class RendererTest_GridRenderer : public Razix::RZApplication
{
public:
    RendererTest_GridRenderer()
        : RZApplication(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Sandbox/")), "RendererTest_ForwardRenderer")
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

    ~RendererTest_GridRenderer() {}

    void OnStart() override
    {
        // Add some model entities
        auto& modelEnitties = Razix::RZEngine::Get().getSceneManager().getCurrentScene()->GetComponentsOfType<Razix::Graphics::RZModel>();
        if (!modelEnitties.size()) {
            // Avocado
            auto& armadilloModelEntity = Razix::RZEngine::Get().getSceneManager().getCurrentScene()->createEntity("Avacado Model");
            armadilloModelEntity.AddComponent<Razix::Graphics::RZModel>("//Meshes/Avocado.gltf");
        }

        // Add a directional light for test
        auto& lightEnitties = Razix::RZEngine::Get().getSceneManager().getCurrentScene()->GetComponentsOfType<Razix::LightComponent>();
        if (!lightEnitties.size()) {
            auto& directionalLightEntity = Razix::RZEngine::Get().getSceneManager().getCurrentScene()->createEntity("Directional Light");
            directionalLightEntity.AddComponent<Razix::LightComponent>();
        }

        auto scripts = Razix::RZEngine::Get().getSceneManager().getCurrentScene()->GetComponentsOfType<Razix::LuaScriptComponent>();
        if (!scripts.size()) {
     
            Razix::RZEntity imguiEntity = Razix::RZEngine::Get().getSceneManager().getCurrentScene()->createEntity("imgui_script_entity");
            imguiEntity.AddComponent<Razix::LuaScriptComponent>();
            if (imguiEntity.HasComponent<Razix::LuaScriptComponent>()) {
                Razix::LuaScriptComponent& lsc = imguiEntity.GetComponent<Razix::LuaScriptComponent>();
                lsc.loadScript("//Scripts/imgui_test.lua");
            }
        }
    }
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new RendererTest_GridRenderer();
}

void main(int argc, char** argv)
{
    EngineMain(argc, argv);
}