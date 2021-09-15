#include "rzxpch.h"

// TODO: Include source files for ImGui back end support depending on the engine's rendering API

//#ifdef OPENGL_RENDERER
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "backends/imgui_impl_opengl3.cpp"
#include "backends/imgui_impl_glfw.cpp"
//#elif DIRECT_3D_RENDERER
// Include direct x specific back end files