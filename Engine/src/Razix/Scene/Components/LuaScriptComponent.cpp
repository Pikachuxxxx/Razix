// clang-format off
#include "rzxpch.h"
// clang-format on
#include "LuaScriptComponent.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Scene/RZEntity.h"

#include "Razix/Scripting/RZLuaScriptHandler.h"

namespace Razix {

    static i32 GetLuaFunctionRefID(lua_State* L, const char* funcName)
    {
        lua_getglobal(L, funcName);
        if (lua_isfunction(L, -1)) {
            return luaL_ref(L, LUA_REGISTRYINDEX);
        } else {
            //lua_pop(L, 1);    // Pop non-function value from stack
            return -1;
        }
    }

    static bool CallLuaFunction(i32 funcRegistryIndex)
    {
        lua_State* L = Scripting::RZLuaScriptHandler::Get().getState();
        lua_rawgeti(L, LUA_REGISTRYINDEX, funcRegistryIndex);
        if (lua_pcall(L, 0, 0, 0) != 0) {
            auto errorStr = lua_tostring(L, -1);
            (void) errorStr;
            RAZIX_CORE_ERROR("[Lua Script Manager] Error : {0}", errorStr);
            lua_pop(L, 1);    // Pop error message from stack
            return false;
        }
        return true;
    }

    void LuaScriptComponent::loadScript(const std::string& scriptPath)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCRIPTING);

        m_Filepath = scriptPath;
        std::string physicalPath;
        if (!RZVirtualFileSystem::Get().resolvePhysicalPath(scriptPath, physicalPath)) {
            RAZIX_CORE_ERROR("[Lua Script Manager] Failed to Load Lua script {0}", scriptPath);
            return;
        } else
            RAZIX_CORE_INFO("[Lua Script Manager] Loading script from : {0}", m_Filepath);

        lua_State* L = Scripting::RZLuaScriptHandler::Get().getState();

        if (luaL_loadfile(L, physicalPath.c_str()) || lua_pcall(L, 0, 0, 0)) {
            auto errorStr = lua_tostring(L, -1);
            (void) errorStr;
            RAZIX_CORE_ERROR("[Lua Script Manager] Failed to Execute Lua script {0}", physicalPath);
            RAZIX_CORE_ERROR("[Lua Script Manager] Error : {0}", errorStr);
            m_Errors.push_back(std::string(errorStr));
            lua_pop(L, 1);    // Pop error message from stack
        }

        m_OnStartFunc = GetLuaFunctionRefID(L, "OnStart");
        m_UpdateFunc  = GetLuaFunctionRefID(L, "OnUpdate");
        m_OnImGuiFunc = GetLuaFunctionRefID(L, "OnImGui");

        // TODO: Load any other functions present in the script
    }

    // TODO: Pass entity as func params

    void LuaScriptComponent::OnStart(RZEntity entity)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCRIPTING);

        if (m_OnStartFunc > -1) {
            bool success = CallLuaFunction(m_OnStartFunc);
            (void) success;
            RAZIX_CORE_ASSERT(success, "[Lua Script Manager] Error in OnStart: {0}", m_Filepath);
        }
    }

    void LuaScriptComponent::OnUpdate(RZEntity entity, RZTimestep dt)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCRIPTING);

        if (m_UpdateFunc > -1) {
            bool success = CallLuaFunction(m_UpdateFunc);
            (void) success;
            RAZIX_CORE_ASSERT(success, "[Lua Script Manager] Error in Update: {0}", m_Filepath);
        }
    }

    void LuaScriptComponent::OnImGui()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCRIPTING);

        if (m_OnImGuiFunc > -1) {
            bool success = CallLuaFunction(m_OnImGuiFunc);
            (void) success;
            RAZIX_CORE_ASSERT(success, "[Lua Script Manager] Error in OnImGui: {0}", m_Filepath);
        }
    }
}    // namespace Razix
