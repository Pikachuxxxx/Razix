#include "rzxpch.h"
#include "ImGuiLayer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
// TODO: Include header files for ImGui back end support depending on the engine's rendering API
//#ifdef OPENGL_RENDERER
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
//#elif DIRECT_3D_RENDERER
// Include direct x specific back end header files

#include "Razix/Application.h"

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
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Unexpected crashing caused by (ImGuiImpl + glfw) API, GLFWwindow* is being passed properly but becomes a nullptr abruptly in the internal API
        // IDK if this is caused by Engine's reference to the native window is being broken or if the internal (ImGuiImpl + GLFW) API is disrupting the reference to the native window

        ImGui::StyleColorsDark();

        // TODO: Use defines to check for the engine's current rendering API and cast to the appropriate type
        Application& app = Application::GetApplication();
        GLFWwindow* window = (GLFWwindow*)(app.GetWindow().GetNativeWindow());

        // This window reference is being casted into null somewhere causing the Razix application to crash when using multiple viewports
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
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
        io.DisplaySize = ImVec2((float)(double)app.GetWindow().GetWidth(), (float)(double)app.GetWindow().GetHeight());

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

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::MenuItem("Item 1", NULL, &show);
                if (ImGui::BeginMenu("Menu 1"))
                {
                    ImGui::MenuItem("item 2", NULL, &show);
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }
}