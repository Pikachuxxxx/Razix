// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZEngineRuntimeTools.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/plugins/IconsFontAwesome5.h>

namespace Razix {
    namespace Tools {

        // FrameGraph Resource Viewer using ImGui with strict clipping and scrollable area
        void OnImGuiDrawFrameGraphVis(const Gfx::FrameGraph::RZFrameGraph& frameGraph)
        {
            ImGui::SetNextWindowBgAlpha(1.0f);
            if (ImGui::Begin("Frame Graph Resource Viewer##FGResourceVis")) {
                ImGui::Text("Welcome to Frame Graph resource viz! Your one stop viewer for Transient resources/Barriers and memory usage of a Frame.");

                const float top_padding = 20.0f;     // Padding at the top
                const float cell_size   = 24.0f;     // Height per row
                const float label_space = 300.0f;    // Width allocated for resource labels
                const float panel_width = label_space - 20.0f;

                const u32 resource_count = frameGraph.getResourceNodesSize();

                // Begin scrollable child region
                ImGui::BeginChild("##ResourceScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

                ImDrawList* draw   = ImGui::GetWindowDrawList();
                ImVec2      origin = ImGui::GetCursorScreenPos() + ImVec2(0, top_padding);

                // Get clipping rectangle for visible area
                const ImVec2 clip_min = ImGui::GetWindowPos();
                const ImVec2 clip_max = ImGui::GetWindowPos() + ImGui::GetWindowSize();

                const ImU32 bg_color_even = IM_COL32(50, 50, 60, 255);
                const ImU32 bg_color_odd  = IM_COL32(40, 40, 50, 255);
                const ImU32 border_color  = IM_COL32(120, 120, 120, 255);
                const ImU32 text_color    = IM_COL32(220, 220, 255, 255);

                // Draw header
                ImVec2 header_p0 = origin;
                ImVec2 header_p1 = origin + ImVec2(panel_width, cell_size);
                draw->AddRectFilled(header_p0, header_p1, IM_COL32(90, 90, 110, 255));
                draw->AddRect(header_p0, header_p1, border_color);
                draw->AddText(ImVec2(header_p0.x + 10.0f, header_p0.y + 8.0f), IM_COL32_WHITE, ICON_FA_BOX_OPEN " Resources");

                // Draw each resource row
                for (u32 ry = 0; ry < resource_count; ++ry) {
                    ImVec2 row_p0 = origin + ImVec2(0, (ry + 1) * cell_size);
                    ImVec2 row_p1 = row_p0 + ImVec2(panel_width, cell_size);

                    // Check if row is outside the visible clip region
                    if (row_p1.y < clip_min.y || row_p0.y > clip_max.y)
                        continue;

                    ImU32 bg_col = (ry % 2 == 0) ? bg_color_even : bg_color_odd;
                    draw->AddRectFilled(row_p0, row_p1, bg_col);
                    draw->AddRect(row_p0, row_p1, border_color);

                    const std::string& name = frameGraph.getResourceName(ry);
                    draw->AddText(ImVec2(row_p0.x + 10.0f, row_p0.y + 6.0f), text_color, name.c_str());

                    ImVec2 mouse = ImGui::GetMousePos();
                    if (mouse.x >= row_p0.x && mouse.x <= row_p1.x && mouse.y >= row_p0.y && mouse.y <= row_p1.y) {
                        ImGui::BeginTooltip();
                        ImGui::Text("Resource ID: %u", ry);
                        ImGui::Separator();
                        ImGui::Text("Type: Texture");
                        ImGui::Text("Usage: Transient");
                        ImGui::Text("Lifetime: Frame 3 - 10");
                        ImGui::Text("Memory: 1.2 MB");
                        ImGui::EndTooltip();
                    }
                }


                ImGui::EndChild();
            }
            ImGui::End();
        }

        //-------------------------------------------------------------------

        void OnImGuiDrawStatusBar()
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
            ImGui::SetNextWindowBgAlpha(1.0f);
            ImGui::SetNextWindowSize(ImVec2((f32) RZApplication::Get().getWindow()->getWidth(), 150.0f));
            ImGui::SetNextWindowPos(ImVec2(0.0f, (f32) RZApplication::Get().getWindow()->getHeight() - 50), ImGuiCond_Always);
            ImGui::Begin("##StatusBar", 0, window_flags);
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
                ImGui::Text(ICON_FA_MEMORY "  GPU Memory: %4.2f", RZEngine::Get().GetStatistics().TotalGPUMemory);
                ImGui::PopStyleColor(1);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
                ImGui::Text(ICON_FA_BALANCE_SCALE " Used GPU Memory: %4.2f |", RZEngine::Get().GetStatistics().GPUMemoryUsed);
                ImGui::PopStyleColor(1);

                ImGui::SameLine();
                std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                ImGui::Text(ICON_FA_CLOCK " current date/time : %s ", std::ctime(&end_time));
                ImGui::PopStyleColor(1);

                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                std::string engineBuildVersionFull = RazixVersion.getVersionString() + "." + RazixVersion.getReleaseStageString();
                ImGui::Text("| Engine build version : %s | ", engineBuildVersionFull.c_str());
                ImGui::PopStyleColor(1);

                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                ImGui::Text(ICON_FA_ID_CARD " project UUID : %s", RZApplication::Get().getProjectUUID().prettyString().c_str());
                ImGui::PopStyleColor(1);
            }
            ImGui::End();
        }

        //-------------------------------------------------------------------

        void OnImGuiDrawEngineTools(ToolsDrawConfig& drawConfig)
        {
            //-------------------------------------------------------------------
            // Engine ImGui Tools will be rendered here

            RAZIX_PROFILE_SCOPEC("Engine Tools", RZ_PROFILE_COLOR_CORE);

            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu(ICON_FA_WRENCH " Tools")) {
                    if (ImGui::MenuItem(ICON_FA_TASKS " FG Resource Vis", NULL, drawConfig.showFrameGraphResourceVis)) {
                        drawConfig.showFrameGraphResourceVis = !drawConfig.showFrameGraphResourceVis;
                    }
                    if (ImGui::MenuItem(ICON_FA_MONEY_BILL " Frame Budgets", NULL, drawConfig.showEngineBudgetBook)) {
                        drawConfig.showEngineBudgetBook = !drawConfig.showEngineBudgetBook;
                    }

                    if (ImGui::MenuItem(ICON_FA_MEMORY " RHI Memory Stats", NULL, drawConfig.showRHIStats)) {
                        drawConfig.showRHIStats = !drawConfig.showRHIStats;
                    }

                    //-----------------------
                    ImGui::Separator();
                    //-----------------------

                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
            //-------------------------------------------------------------------

            if (drawConfig.showStatusBar)
                OnImGuiDrawStatusBar();

            if (drawConfig.showFrameGraphResourceVis) {
                const Gfx::FrameGraph::RZFrameGraph& fg = RZEngine::Get().getWorldRenderer().getFrameGraph();
                OnImGuiDrawFrameGraphVis(fg);
            }
        }
    }    // namespace Tools
}    // namespace Razix
