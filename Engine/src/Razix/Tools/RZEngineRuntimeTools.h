#pragma once

namespace Razix {
    namespace Tools {
        
        struct ToolsConfig
        {
            bool showMemoryInfoStatusBar    = true;
            bool showFrameGraphResourceVis  = false;
            bool showEngineBudgetBook       = false;
            bool showRHIStats               = false;
            bool showResourcePoolsBook      = false;
        };

        /* Draws the runtime ImGui based engine tools */
        void OnImGuiDrawEngineTools(const ToolsConfig& config);

    }
}
