#include "rzxpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
// TODO: Include header files for ImGui back end support depending on the engine's rendering API
//#ifdef OPENGL_RENDERER
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
//#elif DIRECT_3D_RENDERER
// Include direct x specific back end header files


#include "Razix/Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32 
#include <GLFW/glfw3native.h>
namespace Razix
{

    ImGuiLayer::ImGuiLayer()
        :Layer("ImGui Layer")
    {

    }

    ImGuiLayer::~ImGuiLayer()
    {

    }

    void ImGuiLayer::OnAttach()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        // TODO: Use defines to check for the engine's current rendering API and cast to the appropriate type
        Application& app = Application::GetApplication();
        GLFWwindow* window = (GLFWwindow*)(app.GetWindow().GetNativeWindow());

        // This window reference is being casted into null somewhere causing the Razix application to crash
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }

    void ImGuiLayer::OnDetach()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::End()
    {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        Application& app = Application::GetApplication();
        io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backendWindow = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backendWindow);
        }
    }

    void ImGuiLayer::OnImguiRender()
    {
        static bool show = true;
        ImGui::ShowDemoWindow(&show);
    }
}