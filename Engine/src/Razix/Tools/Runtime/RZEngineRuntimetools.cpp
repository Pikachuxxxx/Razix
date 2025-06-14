// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZEngineRuntimeTools.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"

#include "Razix/Utilities/RZStringUtilities.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/plugins/IconsFontAwesome5.h>

namespace Razix {
    namespace Tools {

        //---------------------------------------------------------------------------------------------------------------------
        // Style Definitions

        struct CellStyle
        {
            constexpr static float Roundness       = 4.0f;
            constexpr static float BorderThickness = 1.0f;
            constexpr static ImU32 Color           = IM_COL32(255, 192, 203, 200);    // Light pink w/ alpha
            constexpr static ImU32 HoverColor      = IM_COL32(255, 182, 193, 255);    // Slightly darker
            constexpr static ImU32 BorderColor     = IM_COL32(255, 160, 170, 255);
        };

        struct BarrierStyle
        {
            constexpr static ImU32 ReadFillColor    = IM_COL32(80, 150, 255, 64);
            constexpr static ImU32 WriteFillColor   = IM_COL32(255, 80, 150, 64);
            constexpr static ImU32 ReadBorderColor  = IM_COL32(80, 150, 255, 255);
            constexpr static ImU32 WriteBorderColor = IM_COL32(255, 80, 150, 255);
            constexpr static float BorderThickness  = 2.5f;
            constexpr static float CornerRounding   = 3.0f;
        };

        struct PassLabelStyle
        {
            constexpr static ImU32 TextColor       = IM_COL32(230, 255, 230, 255);
            constexpr static ImU32 ShadowColor     = IM_COL32(20, 20, 20, 180);
            constexpr static ImU32 BgColor         = IM_COL32(60, 80, 60, 200);
            constexpr static ImU32 BorderColor     = IM_COL32(100, 220, 100, 255);
            constexpr static ImU32 ColumnLineColor = IM_COL32(100, 100, 100, 120);
            constexpr static float PaddingX        = 6.0f;
            constexpr static float PaddingY        = 3.0f;
            constexpr static float BoxRounding     = 4.0f;
            constexpr static float BorderThickness = 1.5f;
            constexpr static float Spacing         = 10.0f;
            constexpr static float LineGap         = 8.0f;
            constexpr static float MaxTextWidth    = 150.0f;
        };

        struct ResourcePanelStyle
        {
            constexpr static ImU32 BgColorEven      = IM_COL32(50, 50, 60, 255);
            constexpr static ImU32 BgColorOdd       = IM_COL32(40, 40, 50, 255);
            constexpr static ImU32 BorderColor      = IM_COL32(120, 120, 120, 255);
            constexpr static ImU32 TextColor        = IM_COL32(220, 220, 255, 255);
            constexpr static ImU32 HeaderBgColor    = IM_COL32(100, 80, 150, 255);
            constexpr static ImU32 BarrierBgColor   = IM_COL32(255, 125, 125, 100);
            constexpr static ImU32 BarrierTextColor = IM_COL32(255, 255, 200, 255);
        };

        //---------------------------------------------------------------------------------------------------------------------

        struct SimulatedBarrier
        {
            uint32_t resourceIdx;
            uint32_t passIdx;
            bool     isWrite;
        };

        void DrawLifetimeCell(const ImVec2& pos, float width, float height)
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2      min      = pos;
            ImVec2      max      = ImVec2(pos.x + width, pos.y + height);

            ImVec2 mouse   = ImGui::GetMousePos();
            bool   hovered = mouse.x >= min.x && mouse.x <= max.x && mouse.y >= min.y && mouse.y <= max.y;

            ImU32 color = hovered ? CellStyle::HoverColor : CellStyle::Color;

            drawList->AddRectFilled(min, max, color, CellStyle::Roundness);
            drawList->AddRect(min, max, CellStyle::BorderColor, CellStyle::Roundness, 0, CellStyle::BorderThickness);

            if (hovered)
                ImGui::SetTooltip("%s", "TEST TOOLTIP");
        }

        void DrawSimulatedBarriers(ImDrawList*   draw,
            ImVec2                               origin,
            float                                cellWidth,
            float                                cellHeight,
            const std::vector<SimulatedBarrier>& barriers)
        {
            for (const auto& barrier: barriers) {
                const float x = origin.x + (barrier.passIdx + 1) * cellWidth;
                const float y = origin.y + (barrier.resourceIdx + 1) * cellHeight;

                ImVec2 p0 = ImVec2(x + 6, y + 4);
                ImVec2 p1 = ImVec2(x + cellWidth - 6, y + cellHeight - 4);

                const ImU32 fillColor   = barrier.isWrite ? BarrierStyle::WriteFillColor : BarrierStyle::ReadFillColor;
                const ImU32 borderColor = barrier.isWrite ? BarrierStyle::WriteBorderColor : BarrierStyle::ReadBorderColor;
                const char* label       = barrier.isWrite ? "W" : "R";

                draw->AddRectFilled(p0, p1, fillColor, BarrierStyle::CornerRounding);
                draw->AddRect(p0, p1, borderColor, BarrierStyle::CornerRounding, 0, BarrierStyle::BorderThickness);

                ImVec2 labelPos = ImVec2((p0.x + p1.x) * 0.5f, p0.y + 3);
                ImVec2 textSize = ImGui::CalcTextSize(label);
                draw->AddText(ImVec2(labelPos.x - textSize.x * 0.5f, labelPos.y), borderColor, label);
            }
        }

        void DrawPassLabels(const Gfx::FrameGraph::RZFrameGraph& frameGraph,
            ImDrawList*                                          draw,
            ImVec2                                               origin,
            float                                                cellWidth,
            float                                                cellHeight,
            uint32_t                                             resourcesCount)
        {
            float y           = origin.y;
            float totalHeight = (resourcesCount + 2) * cellHeight;
            float x           = origin.x + 25.0f;
            float boxWidth    = PassLabelStyle::MaxTextWidth + 2.0f * PassLabelStyle::PaddingX;

            draw->AddLine(ImVec2(x, y), ImVec2(x, y + totalHeight), PassLabelStyle::ColumnLineColor);
            x += PassLabelStyle::LineGap;

            const auto& compiledPassNodes = frameGraph.getCompiledPassNodes();
            for (uint32_t i = 0; i < compiledPassNodes.size(); ++i) {
                const auto&        passNode = frameGraph.getPassNode(compiledPassNodes[i]);
                const std::string& label    = Utilities::GetFilePathExtension(passNode.getName());

                float  boxHeight = ImGui::GetFontSize() + 2.0f * PassLabelStyle::PaddingY;
                ImVec2 p0        = ImVec2(x, y);
                ImVec2 p1        = ImVec2(x + boxWidth, y + boxHeight);

                draw->AddRectFilled(p0, p1, PassLabelStyle::BgColor, PassLabelStyle::BoxRounding);
                draw->AddRect(p0, p1, PassLabelStyle::BorderColor, PassLabelStyle::BoxRounding, 0, PassLabelStyle::BorderThickness);

                ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
                ImVec2 textPos  = ImVec2(p0.x + (boxWidth - textSize.x) * 0.5f, p0.y + PassLabelStyle::PaddingY);
                draw->AddText(ImVec2(textPos.x + 1, textPos.y + 1), PassLabelStyle::ShadowColor, label.c_str());
                draw->AddText(textPos, PassLabelStyle::TextColor, label.c_str());

                ImVec2 mouse = ImGui::GetMousePos();
                if (mouse.x >= p0.x && mouse.x <= p1.x && mouse.y >= p0.y && mouse.y <= p1.y) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Pass ID        : %u", passNode.getID());
                    ImGui::Separator();
                    ImGui::Text("Creates        : %d", passNode.getCreatResources().size());
                    ImGui::Text("Reads          : %d", passNode.getInputResources().size());
                    ImGui::Text("Writes         : %d", passNode.getOutputResources().size());
                    ImGui::Separator();
                    auto                  dept = passNode.getDepartment();
                    const DepartmentInfo& info = s_DepartmentInfo.at(dept);
                    ImGui::Text("Department     : %s", info.debugName);
                    ImGui::Separator();
                    auto& budget = passNode.getCurrentPassBudget();
                    ImGui::Text("CPU Budget     : %.2f ms", budget.CPUframeBudget);
                    ImGui::Text("Memory Budget  : %u MiB", budget.MemoryBudget);
                    ImGui::EndTooltip();
                }

                float line_x = x + boxWidth + PassLabelStyle::LineGap;
                draw->AddLine(ImVec2(line_x, y), ImVec2(line_x, y + totalHeight), PassLabelStyle::ColumnLineColor);
                x = line_x + PassLabelStyle::Spacing;
            }
        }

        // Call site: top-level draw function
        void OnImGuiDrawFrameGraphVis(const Gfx::FrameGraph::RZFrameGraph& frameGraph)
        {
            ImGui::SetNextWindowBgAlpha(1.0f);
            if (!ImGui::Begin("Frame Graph Resource Viewer##FGResourceVis")) return;

            ImGui::Text("Welcome to Frame Graph resource viz! Your one stop viewer for Transient resources/Barriers and memory usage of a Frame.");

            const float topPadding = 20.0f;
            const float cellSize   = 24.0f;
            const float labelSpace = 300.0f;
            const float panelWidth = labelSpace - 20.0f;

            std::vector<uint32_t> compiledResourceEntryPoints = frameGraph.getCompiledResourceEntries();
            uint32_t              resourceCount               = compiledResourceEntryPoints.size();

            ImDrawList* draw   = ImGui::GetWindowDrawList();
            ImVec2      origin = ImGui::GetCursorScreenPos() + ImVec2(0, topPadding);

            float    contentHeight  = ImGui::GetContentRegionAvail().y - topPadding;
            uint32_t maxVisibleRows = static_cast<uint32_t>(contentHeight / cellSize);
            uint32_t maxRows        = std::min(resourceCount, maxVisibleRows > 1 ? maxVisibleRows - 2 : 0);

            // Header
            ImVec2 p0 = origin;
            ImVec2 p1 = origin + ImVec2(panelWidth, cellSize);
            draw->AddRectFilled(p0, p1, ResourcePanelStyle::HeaderBgColor);
            draw->AddRect(p0, p1, ResourcePanelStyle::BorderColor);
            draw->AddText(p0 + ImVec2(10.0f, 8.0f), IM_COL32_WHITE, "[Resource/Passes]");

            // Barrier label
            p0 = origin + ImVec2(0, cellSize);
            p1 = p0 + ImVec2(panelWidth, cellSize);
            draw->AddRectFilled(p0, p1, ResourcePanelStyle::BarrierBgColor);
            draw->AddRect(p0, p1, ResourcePanelStyle::BorderColor);
            draw->AddText(p0 + ImVec2(10.0f, 6.0f), ResourcePanelStyle::BarrierTextColor, "[Barriers]");

            for (uint32_t ry = 0; ry < maxRows; ++ry) {
                ImVec2 row_p0 = origin + ImVec2(0, (ry + 2) * cellSize);
                ImVec2 row_p1 = row_p0 + ImVec2(panelWidth, cellSize);

                ImU32 bgColor = (ry % 2 == 0) ? ResourcePanelStyle::BgColorEven : ResourcePanelStyle::BgColorOdd;
                draw->AddRectFilled(row_p0, row_p1, bgColor);
                draw->AddRect(row_p0, row_p1, ResourcePanelStyle::BorderColor);

                const auto& resNode = frameGraph.getResourceNode(compiledResourceEntryPoints[ry]);
                draw->AddText(row_p0 + ImVec2(10.0f, 8.0f), ResourcePanelStyle::TextColor, resNode.getName().c_str());

                ImVec2 mouse = ImGui::GetMousePos();
                if (mouse.x >= row_p0.x && mouse.x <= row_p1.x && mouse.y >= row_p0.y && mouse.y <= row_p1.y) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Resource ID    : %u", resNode.getID());
                    ImGui::Separator();
                    ImGui::Text("EntryPoint ID  : %u", resNode.getResourceEntryId());
                    ImGui::Text("Version        : %u", resNode.getVersion());
                    ImGui::Text("RefCount       : %u", resNode.getRefCount());
                    ImGui::Text("CompiledResIdx : %u", compiledResourceEntryPoints[ry]);
                    ImGui::EndTooltip();
                }
            }

            DrawPassLabels(frameGraph, draw, origin + ImVec2(panelWidth + 20.0f, 0), cellSize, cellSize, maxRows);

            //for (int i = 0; i < 5; ++i) {
            //    DrawLifetimeCell(ImVec2(origin.x + i * (80.0f + 6.0f), origin.y), 80.0f, 18.0f);
            //}

            float drawnHeight = (maxRows + 2) * cellSize + topPadding;
            ImGui::Dummy(ImVec2(panelWidth + 10.0f, drawnHeight));
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
            RAZIX_PROFILE_SCOPEC("Engine Tools", RZ_PROFILE_COLOR_CORE);

            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu(ICON_FA_WRENCH " Tools")) {
                    ImGui::MenuItem(ICON_FA_TASKS " FG Resource Vis", NULL, &drawConfig.showFrameGraphResourceVis);
                    ImGui::MenuItem(ICON_FA_MONEY_BILL " Frame Budgets", NULL, drawConfig.showEngineBudgetBook);
                    ImGui::MenuItem(ICON_FA_MEMORY " RHI Memory Stats", NULL, drawConfig.showRHIStats);
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            if (drawConfig.showStatusBar)
                OnImGuiDrawStatusBar();

            if (drawConfig.showFrameGraphResourceVis) {
                const Gfx::FrameGraph::RZFrameGraph& fg = RZEngine::Get().getWorldRenderer().getFrameGraph();
                OnImGuiDrawFrameGraphVis(fg);
            }
        }
    }    // namespace Tools
}    // namespace Razix
