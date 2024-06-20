#include "RazixGameFrameworkApp.h"

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    return new RazixGameApplication();
}

RAZIX_PLATFORM_MAIN