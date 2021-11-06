#include "rzxpch.h"
#include "RZInput.h"

#include "Razix/Platform/GLFW/GLFWInput.h"

void Razix::RZInput::SelectGLFWInputManager()
{
    sInstance = new GLFWInput;
}
