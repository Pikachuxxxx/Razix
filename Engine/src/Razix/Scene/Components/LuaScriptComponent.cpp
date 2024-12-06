// clang-format off
#include "rzxpch.h"
// clang-format on
#include "LuaScriptComponent.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Scene/RZEntity.h"

#include "Razix/Scripting/RZLuaScriptHandler.h"

namespace Razix {

    void LuaScriptComponent::loadScript(const std::string& scriptPath)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCRIPTING);

        m_Filepath = scriptPath;
        std::string physicalPath;
        if (!RZVirtualFileSystem::Get().resolvePhysicalPath(scriptPath, physicalPath)) {
            RAZIX_CORE_ERROR("[Lua Script Manager] Failed to Load Lua script {0}", scriptPath);
            m_Env = nullptr;
            return;
        } else
            RAZIX_CORE_INFO("[Lua Script Manager] Loading script from : {0}", m_Filepath);

        m_Env = std::make_shared<sol::environment>(Scripting::RZLuaScriptHandler::Get().getState(), sol::create, Scripting::RZLuaScriptHandler::Get().getState().globals());

        auto loadFileResult = Scripting::RZLuaScriptHandler::Get().getState().script_file(physicalPath, *m_Env, sol::script_pass_on_error);
        if (!loadFileResult.valid()) {
            sol::error err = loadFileResult;
            RAZIX_CORE_ERROR("[Lua Script Manager] Failed to Execute Lua script {0}", physicalPath);
            RAZIX_CORE_ERROR("[Lua Script Manager] Error : {0}", err.what());
            m_Errors.push_back(std::string(err.what()));
        }

        m_OnStartFunc = std::make_shared<sol::protected_function>((*m_Env)["OnStart"]);
        if (!m_OnStartFunc->valid())
            m_OnStartFunc.reset();

        m_UpdateFunc = std::make_shared<sol::protected_function>((*m_Env)["OnUpdate"]);
        if (!m_UpdateFunc->valid())
            m_UpdateFunc.reset();

        m_OnImGuiFunc = std::make_shared<sol::protected_function>((*m_Env)["OnImGui"]);
        if (!m_OnImGuiFunc->valid())
            m_OnImGuiFunc.reset();
    }

    void LuaScriptComponent::OnStart(RZEntity entity)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCRIPTING);

        if (m_OnStartFunc) {
            sol::protected_function_result result = m_OnStartFunc->call(entity);
            if (!result.valid()) {
                sol::error err = result;
                RAZIX_CORE_ERROR("Failed to Execute Script Lua Init function");
                RAZIX_CORE_ERROR("Error : {0}", err.what());
            }
        }
    }

    void LuaScriptComponent::OnUpdate(RZEntity entity, RZTimestep dt)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCRIPTING);

        if (m_UpdateFunc) {
            sol::protected_function_result result = m_UpdateFunc->call(entity, dt.GetTimestepMs());
            if (!result.valid()) {
                sol::error err = result;
                RAZIX_CORE_ERROR("Failed to Execute Script Lua OnUpdate");
                RAZIX_CORE_ERROR("Error : {0}", err.what());
            }
        }
    }

    void LuaScriptComponent::OnImGui()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCRIPTING);

        if (m_OnImGuiFunc) {
            sol::protected_function_result result = m_OnImGuiFunc->call();
            if (!result.valid()) {
                sol::error err = result;
                RAZIX_CORE_ERROR("Failed to Execute Script Lua OnImGui");
                RAZIX_CORE_ERROR("Error : {0}", err.what());
            }
        }
    }

}    // namespace Razix