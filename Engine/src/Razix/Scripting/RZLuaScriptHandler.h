#pragma once

#include "Razix/Utilities/TRZSingleton.h"

extern "C"
{
#include <lauxlib.h>    // Provides auxiliary functions for error handling, buffer manipulation, etc.
#include <lua.h>
#include <lualib.h>    // Provides standard libraries like base, string, table, etc.
}

namespace Razix {

    class RZScene;
    class RZTimestep;

    namespace Scripting {

        // TODO: Derive this from the engine system and register to handle automatic startup and shutdown

        /**
         * A class that handled lua script file and their execution, lifetime, memory and Engine API access
         * 
         * This also a Engine system that will be managed by the engine class for startUp and shutdown
         */
        class RAZIX_API RZLuaScriptHandler : public RZSingleton<RZLuaScriptHandler>
        {
        public:
            /* Initializes the system */
            void StartUp();
            /* Shuts down the Lua handler system and releases any resources hold by this */
            void ShutDown();

            void OnStart(RZScene* scene);
            void OnUpdate(RZScene* scene, RZTimestep dt);
            void OnImGui(RZScene* scene);

            lua_State* getState() { return m_State; }

        private:
            lua_State* m_State = nullptr; /* Manages the lua runtime state and executes scripts etc. */

        private:
            /* Binds Application API which the user can control such as Widows system information, or get app state and other simple information */
            void bindApplicationAPI();
            /* Binds the Razix Logging API which the client can use to log in the app using Razix Application Logger */
            void bindLoggingAPI();
            /* Binds the ImGUi API  which the client can use to create ImGui components using lua scripts */
            void bindImGuiAPI();
            /* Binds various renderer public API to lua */
            void bindRendererAPI();

            void bindSceneManagerAPI();
            void bindInputAPI();
            void bindECSAPI();
            void bindglm();
        };
    }    // namespace Scripting
}    // namespace Razix
