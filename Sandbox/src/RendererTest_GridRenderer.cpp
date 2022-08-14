#pragma once
#include <Razix.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

class RendererTest_GridRenderer : public Razix::RZApplication
{
public:
    RendererTest_GridRenderer()
        : RZApplication("/Sandbox/", "RendererTest_GridRenderer")
    {
        Razix::RZInput::SelectGLFWInputManager();
        Razix::RZApplication::Get().Init();

        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
        Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);
        //-------------------------------------------------------------------------------------

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