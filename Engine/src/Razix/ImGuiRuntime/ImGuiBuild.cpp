// clang-format off
#include "rzxpch.h"

// TODO: Include source files for ImGui back end support depending on the engine's rendering API

#ifdef RAZIX_RENDER_API_OPENGL
    #include <imgui/backends/imgui_impl_opengl3.cpp>
#endif

#ifdef RAZIX_RENDER_API_VULKAN
#include <vulkan/vulkan.h>

    #include <imgui/backends/imgui_impl_glfw.cpp>
    #include <imgui/backends/imgui_impl_vulkan.cpp>
#endif



// clang-format on
