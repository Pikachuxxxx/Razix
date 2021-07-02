#include "rzxpch.h"
#include "Engine.h"

#include <chrono>

namespace Razix
{
	void Engine::Ignite()
	{
		auto start = std::chrono::high_resolution_clock::now();

		// Log the Engine Ignition
		RAZIX_CORE_INFO("*************************");
		RAZIX_CORE_INFO("*    Igniting Engine....*");
		RAZIX_CORE_INFO("*************************");

		// Logging the Engine Version details
		RAZIX_CORE_INFO("Version : {0}", Razix::RazixVersion.GetVersionString());
		RAZIX_CORE_INFO("Release Stage : {0}", Razix::RazixVersion.GetReleaseStage());

		// TODO: Ignite all the sub-systems here

		// Log after all the Engine systems have been successfully Started Up
		RAZIX_CORE_INFO("*************************");
		RAZIX_CORE_INFO("*    Engine Ignited!    *");
		RAZIX_CORE_INFO("*************************");

		//TODO: Log the time take to initialize engine using Profiling macros
		auto stop = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> ms_double = (stop - start);
		RAZIX_CORE_INFO("Engine Ingnited in : {0} ms", ms_double.count());
	}

	void Engine::ShutDown()
	{
		UNIMPLEMENTED
	}

	void Engine::Run()
	{
		UNIMPLEMENTED
	}
}