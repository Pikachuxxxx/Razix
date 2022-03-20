#pragma once

#include "Razix/Graphics/Renderers/IRZRenderer.h"

namespace Razix {

    namespace Graphics {
        
        /**
         * ImGui renderer for the Razix engine, manages everything necessary
         * 
         * Note: Used GLFW for events, once engine wide common Input-platform system is done we can use that to redirect events to ImGui controls 
         * such as for consoles etc.
         */
        class RZImGuiRenderer : public IRZRenderer
        {
        };

    }
}