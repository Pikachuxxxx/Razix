#pragma once

#include "Razix/Core/OS/OS.h"

namespace Razix
{
	/// <summary>
	/// Windows specific OS Instance 
	/// </summary>
	class RAZIX_API WindowsOS : public OS
	{
	public:
		WindowsOS() = default;
		~WindowsOS() = default;

		/// <summary>
		/// Initializes the OS resources required for the engine
		/// </summary>
		void Init();
		
		/// <summary>
		/// Initializes the Application and controls it's lifecycle as well as the Engine Runtime systems
		/// This way the OS can still have the master control over the engine's Lifecycle, VFS
		/// By doing this the OS is lending it's resources to the engine while still keeping in control of 
		/// the application to preserve it's master status over the application that it permits to run
		/// </summary>
		void Run() override;

	};
}


