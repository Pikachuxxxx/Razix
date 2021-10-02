#pragma once

#include "Razix/Core/Core.h"

#include "Razix/Utilities/Timestep.h"
#include "Razix/Utilities/TRazixSingleton.h"

#include <string>

namespace Razix
{
	/*
	 * An Interface for Engine Systems, Managers, Libraries and Factories
	 */
	class RAZIX_API IRazixSystem
	{
        IRazixSystem() = default;
        virtual ~IRazixSystem() = default;

        /* StarUp of Engine system and it's behavior */
        virtual void StartUp() = 0;
        virtual void Shutdown() = 0;

        /**
         * Systems Runtime behavior is defined here
         * 
         * @param dt delta time between frames
         */
        virtual void OnUpdate(const Timestep& dt) = 0;
        /* ImGui behavior for the engine system */
        virtual void OnImGui() = 0;

        /* Returns the name of the system for debugging purpose */
        inline const std::string& GetName() const { return m_SystemName; }

    protected:
        std::string m_SystemName;
	};
}