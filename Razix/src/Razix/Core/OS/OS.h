#pragma once

#include "Razix/Core/Core.h"

namespace Razix
{
	/// <summary>
	/// The OS instance helps the Engine get/set information or make changes to the Engine and it's application state 
	/// </summary>
	class RAZIX_API OS
	{
	public:
		OS() = default;
		~OS() = default;

		/// Destroys the OS instance 
		static void Destroy();

		///	Gets the global static OS instance
		static OS* GetInstance() { return s_Instance; }
		/// <summary>
		/// Sets the current instance of the OS
		/// </summary>
		/// <param name="instance"> pointer to the OS instance </param>
		static void SetInstance(OS* instance) { s_Instance = instance; }

		/*** Important ***/
		/// <summary>
		/// Controls the Application lifecycle 
		/// </summary>
		virtual void Run() = 0;
	protected:
		static OS* s_Instance;
	};
}

