// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZInput.h"

#include "Razix/Platform/GLFW/GLFWInput.h"

Razix::RZInput* Razix::RZInput::s_Instance = NULL;

void Razix::RZInput::SelectGLFWInputManager()
{
#ifdef RAZIX_USE_GLFW_WINDOWS
    s_Instance = new GLFWInput;
#endif
}
