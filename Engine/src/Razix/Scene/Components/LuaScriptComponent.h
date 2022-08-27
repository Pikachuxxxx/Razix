#pragma once

#include "Razix/Utilities/RZTimestep.h"

namespace Razix {

    class RAZIX_API LuaScriptComponent
    {
    public:
        LuaScriptComponent()                          = default;
        LuaScriptComponent(const LuaScriptComponent&) = default;
        virtual ~LuaScriptComponent() {}

        /**
            * Loads the lua script file
            * 
            * @param scriptPath The path to where the lua script file is at
            */
        void loadScript(const std::string& scriptPath);

        void OnStart();
        void OnUpdate(RZTimestep dt);
        void OnImGui();

        template<typename Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("FilePath", m_Filepath));
        }

        template<typename Archive>
        void load(Archive& archive)
        {
            archive(cereal::make_nvp("FilePath", m_Filepath));
            loadScript(m_Filepath);
        }

    private:
        std::string m_Filepath;

        std::vector<std::string> m_Errors;

        std::shared_ptr<sol::environment>        m_Env;
        std::shared_ptr<sol::protected_function> m_OnStartFunc;
        std::shared_ptr<sol::protected_function> m_UpdateFunc;
        std::shared_ptr<sol::protected_function> m_OnImGuiFunc;
    };
}    // namespace Razix
