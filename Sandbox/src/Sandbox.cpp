#include <Razix.h>

using namespace Razix;

class Sandbox : public Razix::RZApplication
{
public:
    Sandbox()
        : RZApplication(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Sandbox/")), "SponzaSandbox")
    //        : RZApplication(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Sandbox/")), "ShadowsSandbox")

    {

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
