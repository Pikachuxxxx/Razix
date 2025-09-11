
#include <Razix.h>

using namespace Razix;
// TODO: Name is given by the build system + root dir as well
class RazixGameApplication : public Razix::RZApplication
{
public:
    RazixGameApplication()
        : RZApplication(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Game/")), "RazixGame")

    {
        Razix::RZApplication::Get().Init();

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

    void OnUpdate(const RZTimestep& dt) override
    {
    }

    void OnRender() override
    {
    }

    void OnResize(u32 width, u32 height) override
    {
    }
};