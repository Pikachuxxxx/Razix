// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZWindow.h"

namespace Razix {
    RZWindow* (*RZWindow::ConstructionFunc) (const WindowProperties&) = nullptr;

    RZWindow* RZWindow::Create (const WindowProperties& properties /*= WindowProperties()*/)
    {
        RAZIX_ASSERT (ConstructionFunc, "Windows construction funcition not found!");
        return ConstructionFunc (properties);
    }
}    // namespace Razix