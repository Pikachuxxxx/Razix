#include <Razix.h>

using namespace Razix;

class Sandbox : public Razix::RZApplication
{
public:
    Sandbox()
        //: RZApplication(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Sandbox/")), "SponzaSandbox")
        : RZApplication(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Sandbox/")), "ShadowsSandbox")

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
