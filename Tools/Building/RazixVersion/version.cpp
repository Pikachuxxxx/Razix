/**
 * Prints the Razix Engine Version information
 */

#include <Razix/Core/Version/RazixVersion.h>
#include <iostream>
#include <string>

int main()
{
    // Ex. Razix Engine - 1.2.6.rc
    std::string versionString = "Razix Engine - " + Razix::RazixVersion.getVersionString() + "." + Razix::RazixVersion.getReleaseStageString();    // + " | " + STRINGIZE(RAZIX_BUILD_CONFIG);
    std::cout << versionString << std::endl;
}
