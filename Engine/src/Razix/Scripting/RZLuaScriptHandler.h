#pragma once

#include "Razix/Utilities/TRZSingleton.h"

// Lua and c++ wrapper dependencies

namespace Razix {

    class RZScene;
    class RZTimestep;

    namespace Scripting {

        // TODO: Derive this from the engine system and register to handle automatic startup and shutdown
        // TODO: Add OnImGui rendering feature for it

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

            sol::state& getState() { return m_State; }

        private:
            sol::state m_State; /* Manages the lua runtime state and executes scripts etc. */

        private:
            /* Binds Application API which the user can control such as Widows system information, or get app state and other simple information */
            void bindApplicationAPI();
            /* Binds the Razix Logging API which the client can use to log in the app using Razix Application Logger */
            void bindLoggingAPI();
            /* Binds the ImGUi API  which the client can use to create ImGui components using lua scripts */
            void bindImGuiAPI();
            void bindSceneManagerAPI();
            void bindInputAPI();
            void bindECSAPI();
            void bindglm();
        };
    }    // namespace Scripting
}    // namespace Razix
