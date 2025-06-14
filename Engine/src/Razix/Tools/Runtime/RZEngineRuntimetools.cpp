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
        struct SimulatedBarrier
        {
            uint32_t resourceIdx;
            uint32_t passIdx;
            bool     isWrite;
        };

        static void DrawSimulatedBarriers(ImDrawList* draw,
            ImVec2                                    origin,
            float                                     cellWidth,
            float                                     cellHeight,
            const std::vector<SimulatedBarrier>&      barriers,
            const std::vector<std::string>&           passNames,
            const std::vector<std::string>&           resourceNames)
        {
            const ImU32 readBorderColor  = IM_COL32(80, 150, 255, 255);
            const ImU32 writeBorderColor = IM_COL32(255, 80, 150, 255);
            const ImU32 readFillColor    = IM_COL32(80, 150, 255, 64);
            const ImU32 writeFillColor   = IM_COL32(255, 80, 150, 64);
            const float borderThick      = 2.5f;
            const float cornerRounding   = 3.0f;

            for (const auto& barrier: barriers) {
                const float x = origin.x + (barrier.passIdx + 1) * cellWidth;
                const float y = origin.y + (barrier.resourceIdx + 1) * cellHeight;

                ImVec2 p0 = ImVec2(x + 6, y + 4);
                ImVec2 p1 = ImVec2(x + cellWidth - 6, y + cellHeight - 4);

                const ImU32 fillColor   = barrier.isWrite ? writeFillColor : readFillColor;
                const ImU32 borderColor = barrier.isWrite ? writeBorderColor : readBorderColor;
                const char* label       = barrier.isWrite ? "W" : "R";

                draw->AddRectFilled(p0, p1, fillColor, cornerRounding);
                draw->AddRect(p0, p1, borderColor, cornerRounding, 0, borderThick);

                ImVec2 labelPos = ImVec2((p0.x + p1.x) * 0.5f, p0.y + 3);
                ImVec2 textSize = ImGui::CalcTextSize(label);
                draw->AddText(ImVec2(labelPos.x - textSize.x * 0.5f, labelPos.y), borderColor, label);
            }
        }

        void DrawCyberpunkLifetimes(ImDrawList* draw, ImVec2 origin, float cellWidth, float cellHeight, const std::vector<std::pair<u32, u32>>& lifetimes, u32 passCount, float offsetX)
        {
            const ImU32 lifetime_bg_color = IM_COL32(0, 255, 200, 90);     // Neon aqua glow
            const ImU32 lifetime_border   = IM_COL32(0, 255, 255, 180);    // Cyan outline
            const float border_thickness  = 1.8f;
            const float rounding          = 5.0f;
            const float glow_padding      = 2.0f;
        }

        // Sample pass names
        static const std::vector<std::string> kPassNames = {
            "DepthOnly", "GBuffer", "DeferredLighting", "TAA", "ToneMapping", "Debug", "ImGuI"};
        static const std::vector<float> kPassDurations = {
            1.0f,
            1.0f,
            1.0f,
            0.5f,
            0.5f,
            1.0f,
            1.0f,
        };
        void DrawPassLabels(ImDrawList* draw, ImVec2 origin, float cellWidth, float cellHeight, u32 resourcesCount,
            const std::vector<std::string>& passNames, const std::vector<float>& passDurations)
        {
            const ImU32 label_text_color   = IM_COL32(230, 255, 230, 255);    // Minty green
            const ImU32 label_shadow_color = IM_COL32(20, 20, 20, 180);
            const ImU32 label_bg_color     = IM_COL32(60, 80, 60, 200);       // Dark greenish background
            const ImU32 label_border_color = IM_COL32(100, 220, 100, 255);    // Bright mint border
            const ImU32 column_line_color  = IM_COL32(100, 100, 100, 120);

            const float padding_x             = 6.0f;
            const float padding_y             = 3.0f;
            const float rounding              = 4.0f;
            const float border_thickness      = 1.5f;
            const float spacing_between_boxes = 10.0f;
            const float line_gap              = 8.0f;    // gap between line and box

            float y                  = origin.y;
            float total_label_height = (resourcesCount + 2) * cellHeight;

            // Max text width for fixed-size label boxes
            float max_text_width = 0.0f;
            for (const auto& label: passNames)
                max_text_width = std::max(max_text_width, ImGui::CalcTextSize(label.c_str()).x);

            const float box_width = max_text_width + 2.0f * padding_x;

            float x = origin.x + 25.0f;

            // Draw first vertical line
            draw->AddLine(ImVec2(x, origin.y), ImVec2(x, origin.y + total_label_height), column_line_color);
            x += line_gap;

            for (u32 i = 0; i < passNames.size(); ++i) {
                const std::string& label      = passNames[i];
                float              duration   = passDurations[i];
                float              box_height = ImGui::GetFontSize() + 2.0f * padding_y;

                ImVec2 p0 = ImVec2(x, y);
                ImVec2 p1 = ImVec2(x + box_width, y + box_height);

                // Label background and border
                draw->AddRectFilled(p0, p1, label_bg_color, rounding);
                draw->AddRect(p0, p1, label_border_color, rounding, 0, border_thickness);

                // Centered text
                ImVec2 text_size = ImGui::CalcTextSize(label.c_str());
                ImVec2 text_pos  = ImVec2(p0.x + (box_width - text_size.x) * 0.5f, p0.y + padding_y);
                draw->AddText(ImVec2(text_pos.x + 1, text_pos.y + 1), label_shadow_color, label.c_str());
                draw->AddText(text_pos, label_text_color, label.c_str());

                // Tooltip on hover
                ImVec2 mouse = ImGui::GetMousePos();
                if (mouse.x >= p0.x && mouse.x <= p1.x && mouse.y >= p0.y && mouse.y <= p1.y)
                    ImGui::SetTooltip("Duration: %.2f ms\nTimeline: %.1f%%", duration, duration * 100.0f / 16.6f);

                // Draw vertical line after this box
                float line_x = x + box_width + line_gap;
                draw->AddLine(ImVec2(line_x, origin.y), ImVec2(line_x, origin.y + total_label_height), column_line_color);

                // Advance x: box + spacing + line
                x = line_x + spacing_between_boxes;
            }
        }

        void OnImGuiDrawFrameGraphVis(const Gfx::FrameGraph::RZFrameGraph& frameGraph)
        {
            ImGui::SetNextWindowBgAlpha(1.0f);
            if (ImGui::Begin("Frame Graph Resource Viewer##FGResourceVis")) {
                ImGui::Text("Welcome to Frame Graph resource viz! Your one stop viewer for Transient resources/Barriers and memory usage of a Frame.");

                const float top_padding = 20.0f;
                const float cell_size   = 24.0f;
                const float label_space = 300.0f;
                const float panel_width = label_space - 20.0f;

                const u32 resource_count = frameGraph.getResourceNodesSize();

                ImDrawList* draw   = ImGui::GetWindowDrawList();
                ImVec2      origin = ImGui::GetCursorScreenPos() + ImVec2(0, top_padding);

                float content_region_height = ImGui::GetContentRegionAvail().y - top_padding;
                u32   max_visible_rows      = static_cast<u32>(content_region_height / cell_size);
                u32   max_rows              = std::min(resource_count, max_visible_rows > 1 ? max_visible_rows - 2 : 0);

                // Colors
                const ImU32 bg_color_even      = IM_COL32(50, 50, 60, 255);
                const ImU32 bg_color_odd       = IM_COL32(40, 40, 50, 255);
                const ImU32 border_color       = IM_COL32(120, 120, 120, 255);
                const ImU32 text_color         = IM_COL32(220, 220, 255, 255);
                const ImU32 barrier_bg_color   = IM_COL32(255, 125, 125, 100);
                const ImU32 barrier_text_color = IM_COL32(255, 255, 200, 255);
                const ImU32 header_bg_color    = IM_COL32(100, 80, 150, 255);

                // Header row
                ImVec2 header_p0 = origin;
                ImVec2 header_p1 = origin + ImVec2(panel_width, cell_size);
                draw->AddRectFilled(header_p0, header_p1, header_bg_color);
                draw->AddRect(header_p0, header_p1, border_color);
                draw->AddText(ImVec2(header_p0.x + 10.0f, header_p0.y + 8.0f), IM_COL32_WHITE, ICON_FA_BOX_OPEN " Resource/Passes");

                // Barrier row
                ImVec2 barrier_row_p0 = origin + ImVec2(0, cell_size);
                ImVec2 barrier_row_p1 = barrier_row_p0 + ImVec2(panel_width, cell_size);
                draw->AddRectFilled(barrier_row_p0, barrier_row_p1, barrier_bg_color);
                draw->AddRect(barrier_row_p0, barrier_row_p1, border_color);
                draw->AddText(ImVec2(barrier_row_p0.x + 10.0f, barrier_row_p0.y + 6.0f), barrier_text_color, "[Barriers]");

                // Resource rows
                for (u32 ry = 0; ry < max_rows; ++ry) {
                    ImVec2 row_p0 = origin + ImVec2(0, (ry + 2) * cell_size);
                    ImVec2 row_p1 = row_p0 + ImVec2(panel_width, cell_size);

                    ImU32 bg_col = (ry % 2 == 0) ? bg_color_even : bg_color_odd;
                    draw->AddRectFilled(row_p0, row_p1, bg_col);
                    draw->AddRect(row_p0, row_p1, border_color);

                    const std::string& name     = frameGraph.getResourceName(ry);
                    ImVec2             text_pos = ImVec2(row_p0.x + 10.0f, row_p0.y + 8.0f);
                    draw->AddText(text_pos, text_color, name.c_str());

                    ImVec2 mouse = ImGui::GetMousePos();
                    if (mouse.x >= row_p0.x && mouse.x <= row_p1.x &&
                        mouse.y >= row_p0.y && mouse.y <= row_p1.y) {
                        ImGui::SetTooltip("Resource ID: %u\nUsage: Transient\nMemory: 1.2 MB", ry);
                    }
                }

                // Pass names along top X-axis
                DrawPassLabels(draw, origin + ImVec2(panel_width + 20.0f, 0), cell_size, cell_size, max_rows, kPassNames, kPassDurations);

                float drawn_height = (max_rows + 2) * cell_size + top_padding;
                ImGui::Dummy(ImVec2(panel_width + 10.0f, drawn_height));
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
