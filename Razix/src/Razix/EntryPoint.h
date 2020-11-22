#pragma once

#ifdef RZX_PLATFORM_WINDOWS

// Forward declaring the application creating function
extern Razix::Application* Razix::CreateApplication();

int main(int argc, char** argv)
{
	/// Engine Systems Initializations
	
	/// Logging System
	Razix::Log::InitLogger();

	RZX_CORE_INFO("Initialized Core Engine Logger");
	RZX_INFO("Initialized Engine Application Logger");

	/// Application auto Initialization by the Engine
	auto app = Razix::CreateApplication();
	app->Run();
	delete app;
}

#endif