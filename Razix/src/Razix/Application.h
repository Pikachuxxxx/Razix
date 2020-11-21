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
	Application* CreateApplication();
}

 