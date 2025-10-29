#pragma once

#include "Razix/Core/RZCore.h"

#include "Razix/Core/Utils/RZTimestep.h"
#include "Razix/Core/Utils/TRZSingleton.h"

#include <string>

namespace Razix {
    /*
	 * An Interface for Engine Systems, Managers, Libraries and Factories
	 */
    template<typename T>
    class RAZIX_API IRZSystem
    {
    public:
        IRZSystem()          = default;
        virtual ~IRZSystem() = default;

        /* StarUp of Engine system and it's behavior */
        virtual void StartUp()  = 0;
        virtual void Shutdown() = 0;

        /**
         * Systems Runtime behavior is defined here
         * 
         * @param dt delta time between frames
         */
        virtual void OnUpdate(const RZTimestep& dt) = 0;
        /* ImGui behavior for the engine system */
        virtual void OnImGui() {}

        /* Returns the name of the system for debugging purpose */
        inline const std::string& GetName() const { return m_SystemName; }

    protected:
        std::string m_SystemName;
    };
}    // namespace Razix
