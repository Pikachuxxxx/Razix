// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZInput.h"

#include "Razix/Platform/GLFW/GLFWInput.h"

void Razix::RZInput::SelectGLFWInputManager()
{
    s_Instance = new GLFWInput;
}
