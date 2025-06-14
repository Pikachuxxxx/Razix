#pragma once

namespace Razix {
    namespace Tools {

        struct ToolsDrawConfig
        {
            bool showStatusBar             = true;
            bool showFrameGraphResourceVis = true;
            bool showEngineBudgetBook      = false;
            bool showRHIStats              = false;
            bool showResourcePoolsBook     = false;
        };

        /* Draws the runtime ImGui based engine tools */
        void OnImGuiDrawEngineTools(ToolsDrawConfig& config);

    }    // namespace Tools
}    // namespace Razix
