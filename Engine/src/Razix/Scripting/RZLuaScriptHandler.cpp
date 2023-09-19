// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZLuaScriptHandler.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZSplashScreen.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Scene/Components/LuaScriptComponent.h"

#include "Razix/Utilities/RZTimestep.h"

namespace Razix {
    namespace Scripting {

        void RZLuaScriptHandler::StartUp()
        {
            // Instance is automatically created once the system is Started Up
            RAZIX_CORE_INFO("[Lua Script Manager] Starting Up Lua Script Handler");
            Razix::RZSplashScreen::Get().setLogString("Starting Lua Script Handler...");

            // Load lua default libraries that can be used by the client
            m_State.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table);

            // Bind the Engine Systems Scripting API to be exposed to user via lua
            bindApplicationAPI();

            // Bind the logging API that can be used by the user via scripts
            bindLoggingAPI();

            // Bind the ImGui bindings
            bindImGuiAPI();

            // Bind the Renderer API
            bindRendererAPI();

            // Binding GLM
            bindglm();

            // Bind the ECS API
            bindECSAPI();

            // TODO: Bind other APIs (engine, gfx, ecs, physics etc.)
        }

        void RZLuaScriptHandler::ShutDown()
        {
            RAZIX_CORE_ERROR("[Lua Script Manager] Shutting Lua Script Handler");
        }

        void RZLuaScriptHandler::OnStart(RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCRIPTING);

            auto& registry = scene->getRegistry();

            auto view = registry.view<LuaScriptComponent>();

            if (view.empty())
                return;

            for (auto entity: view) {
                auto& luaScript = registry.get<LuaScriptComponent>(entity);
                luaScript.OnStart(RZEntity(entity, scene));
            }
        }

        void RZLuaScriptHandler::OnUpdate(RZScene* scene, RZTimestep dt)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCRIPTING);

            auto& registry = scene->getRegistry();

            auto view = registry.view<LuaScriptComponent>();

            if (view.empty())
                return;

            for (auto entity: view) {
                auto& luaScript = registry.get<LuaScriptComponent>(entity);
                luaScript.OnUpdate(RZEntity(entity, scene) , dt);
            } 
        }

        void RZLuaScriptHandler::OnImGui(RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCRIPTING);

            auto& registry = scene->getRegistry();

            auto view = registry.view<LuaScriptComponent>();

            if (view.empty())
                return;

            for (auto entity: view) {
                auto& luaScript = registry.get<LuaScriptComponent>(entity);
                luaScript.OnImGui();
            }
        }

        void RZLuaScriptHandler::bindSceneManagerAPI()
        {
        }

        void RZLuaScriptHandler::bindInputAPI()
        {
        }
    }    // namespace Scripting
}    // namespace Razix