#pragma once

#include "Razix/Core/Utils/TRZSingleton.h"

extern "C"
{
#include <lauxlib.h>    // Provides auxiliary functions for error handling, buffer manipulation, etc.
#include <lua.h>
#include <lualib.h>    // Provides standard libraries like base, string, table, etc.
}

// Using macros to register types and functions/methods

#define LUA_CREATE_TABLE(L, table_name) \
    lua_newtable(L);                    \
    lua_setglobal(L, table_name);

#define LUA_REGISTER_GLOBAL_FUNCTION(L, table, func_name, ...) \
    {                                                          \
        auto func = [](lua_State* L) -> int {                  \
            __VA_ARGS__                                        \
        };                                                     \
        lua_getglobal(L, table);                               \
        lua_pushcfunction(L, func);                            \
        lua_setfield(L, -2, #func_name);                       \
        lua_pop(L, 1);                                         \
    }

#define LUA_REGISTER_CLASS_FUNCTION(L, table, class_name, method_name, func_body)         \
    int class_name##_##method_name(lua_State* L)                                          \
    {                                                                                     \
        class_name* obj = *(class_name**) luaL_checkudata(L, 1, #class_name "MetaTable"); \
        func_body                                                                         \
    }                                                                                     \
    lua_getglobal(L, table);                                                              \
    lua_pushcfunction(L, class_name##_##method_name);                                     \
    lua_setfield(L, -2, #method_name);                                                    \
    lua_pop(L, 1);

namespace Razix {

    class RZScene;
    class RZTimestep;

    namespace Scripting {

        // TODO: Derive this from the engine system and register to handle automatic startup and shutdown

        /**
         * A class that handles lua script files and their execution, lifetime, memory and Engine API access
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
