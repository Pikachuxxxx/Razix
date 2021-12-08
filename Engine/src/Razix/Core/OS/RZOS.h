#pragma once

#include "Razix/Core/RZCore.h"

namespace Razix
{
	/* The OS instance helps the Engine get / set information or make changes to the Engine and it's application state */
	class RAZIX_API RZOS
	{
	public:
		RZOS() = default;
		~RZOS() = default;

		/* Destroys the OS instance */
		static void Destroy();

		/* Gets the global static OS instance */
		static RZOS* GetInstance() { return s_Instance; }
		/**
		 * Sets the current instance of the OS
		 * 
		 * @param instance pointer to the OS instance
		 */
		static void SetInstance(RZOS* instance) { s_Instance = instance; }

		/* Initializes the OS instance */
		virtual void Init() = 0;

		 /** Important
		 * Controls the Application lifecycle
		 * Should be Implemented OS so as to give the OS control over the application
		 */
		virtual void Run() = 0;
	protected:
		/* The global OS instance for the Engine to call */
		static RZOS* s_Instance;
	};
}

