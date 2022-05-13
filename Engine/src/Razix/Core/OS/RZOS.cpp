// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZOS.h"

namespace Razix {
    RZOS* RZOS::s_Instance = nullptr;

    void RZOS::Destroy()
    {
        delete s_Instance;
        s_Instance = nullptr;
    }
}    // namespace Razix
