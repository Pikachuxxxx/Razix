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
    }

    void OnRender() override
    {
    }
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Tanu Application");
    return new TanuGameApp();
}

RAZIX_PLATFORM_MAIN
