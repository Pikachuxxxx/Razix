// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Scripting/RZLuaScriptHandler.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

namespace Razix {
    namespace Scripting {

        void RZLuaScriptHandler::bindLoggingAPI()
        {
            auto log = m_State.create_table("RZLog");

            log.set_function("Trace", [&](sol::this_state s, std::string_view message) { RAZIX_TRACE(message); });

            log.set_function("Info", [&](sol::this_state s, std::string_view message) { RAZIX_INFO(message); });

            log.set_function("Warn", [&](sol::this_state s, std::string_view message) { RAZIX_WARN(message); });

            log.set_function("Error", [&](sol::this_state s, std::string_view message) { RAZIX_ERROR(message); });
        }
    }    // namespace Scripting
}    // namespace Razix