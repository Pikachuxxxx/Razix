#pragma once

#include "Razix/Core/Utils/RZTimestep.h"

#include <cereal/cereal.hpp>

namespace Razix {

    class RZEntity;

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
        void loadScript(const RZString& scriptPath);

        void OnStart(RZEntity entity);
        void OnUpdate(RZEntity entity, RZTimestep dt);
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

        const RZString& getScriptFilePath() { return m_Filepath; }

    private:
        RZString              m_Filepath;
        RZDynamicArray<RZString> m_Errors;

        i32 m_OnStartFunc = -1;
        i32 m_UpdateFunc  = -1;
        i32 m_OnImGuiFunc = -1;
    };
}    // namespace Razix
