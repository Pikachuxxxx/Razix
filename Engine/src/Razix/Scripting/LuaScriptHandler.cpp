#include "rzxpch.h"
#include "LuaScriptHandler.h"

#include "Razix/Core/RZSplashScreen.h"
#include "Razix/Core/RZApplication.h"

namespace Razix {
    namespace Scripting {

        void LuaScriptHandler::StartUp()
        {
            // Instance is automatically created once the system is Started Up
            RAZIX_CORE_INFO("[Lua Handler] Starting Up Lua Script Handler");
            Razix::RZSplashScreen::Get().setLogString("Starting Lua Script Handler...");

            // Load lua default libraries that can be used by the client
            m_State.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table);

            // Bind the Engine Systems Scripting API to be exposed to user via lua
            bindApplicationAPI();

            // Bind the logging API that can be used by the user via scripts
            bindLoggingAPI();
        }

        void LuaScriptHandler::ShutDown()
        {
             RAZIX_CORE_ERROR("[Lua Handler] Shutting Lua Script Handler");
        }

        void LuaScriptHandler::bindApplicationAPI()
        {
            sol::usertype<RZApplication> appType = m_State.new_usertype<RZApplication>("RZApplication");

            appType.set_function("GetWindowSize", &RZApplication::getWindowSize);
            m_State.set_function("GetAppInstance", &RZApplication::Get);
        }

        void LuaScriptHandler::bindLoggingAPI()
        {
            auto log = m_State.create_table("RZLog");

            log.set_function("Trace", [&](sol::this_state s, std::string_view message)
            { RAZIX_TRACE(message); });

            log.set_function("Info", [&](sol::this_state s, std::string_view message)
            { RAZIX_INFO(message); });

            log.set_function("Warn", [&](sol::this_state s, std::string_view message)
            { RAZIX_WARN(message); });

            log.set_function("Error", [&](sol::this_state s, std::string_view message)
            { RAZIX_ERROR(message); });
        }

        void LuaScriptHandler::bindSceneManagerAPI()
        {

        }

        void LuaScriptHandler::bindInputAPI()
        {

        }

        void LuaScriptHandler::bindECSAPI()
        {

        }

    }
}