// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Scripting/RZLuaScriptHandler.h"

#include "Razix/Core/App/RZApplication.h"

namespace Razix {
    namespace Scripting {

        void RZLuaScriptHandler::bindLoggingAPI()
        {
            lua_State* L = m_State;

            LUA_CREATE_TABLE(L, "RZLog");

            LUA_REGISTER_GLOBAL_FUNCTION(L, "RZLog", Trace, {
                const char* message = luaL_checkstring(L, 1);
                RAZIX_TRACE(message);
                return 0;
            });

            LUA_REGISTER_GLOBAL_FUNCTION(L, "RZLog", Info, {
                const char* message = luaL_checkstring(L, 1);
                RAZIX_INFO(message);
                return 0;
            });

            LUA_REGISTER_GLOBAL_FUNCTION(L, "RZLog", Warn, {
                const char* message = luaL_checkstring(L, 1);
                RAZIX_WARN(message);
                return 0;
            });

            LUA_REGISTER_GLOBAL_FUNCTION(L, "RZLog", Error, {
                const char* message = luaL_checkstring(L, 1);
                RAZIX_ERROR(message);
                return 0;
            });
        }
    }    // namespace Scripting
}    // namespace Razix