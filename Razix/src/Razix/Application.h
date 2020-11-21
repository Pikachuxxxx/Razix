#pragma once

#include "Core.h"

namespace Razix
{
	class RAZIX_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	/// To be defined on CLIENT side to create the application
	// What this means is that the Entry point thinks the engine will define this for sure.
	// In fact it does by just declaring, but who actually defines it?
	// Now the engine forces the client to implement this according to their needs. 
	// [Entry Point(forward declaration)-->Application(declaration)-->CLIENT(definition)]
	Application* CreateApplication();
}

 