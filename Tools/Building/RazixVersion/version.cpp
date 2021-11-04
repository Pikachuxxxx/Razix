/**
 * Prints the Razix Engine Version information
 */

#include <Razix/Core/RazixVersion.h>
#include <string>
#include <iostream>

int main() 
{
    std::string versionString = "Razix Engine - " + Razix::RazixVersion.getVersionString() + "." + Razix::RazixVersion.getReleaseStageString();// + " | " + STRINGIZE(RAZIX_BUILD_CONFIG);

    std::cout << versionString << std::endl;
}