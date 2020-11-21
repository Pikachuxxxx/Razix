#pragma once

#include <memory>

#include "Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Razix
{
	class RAZIX_API Log
	{
	public:
		static void InitLogger();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetApplicationLogger() { return s_ApplicationLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ApplicationLogger;
	};
}

//#ifndef RZX_BUILD_DIST

/// Core Engine Logging 
#define RZX_CORE_TRACE(...)		::Razix::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RZX_CORE_INFO(...)		::Razix::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RZX_CORE_WARN(...)		::Razix::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RZX_CORE_ERROR(...)		::Razix::Log::GetCoreLogger()->error(__VA_ARGS__)
														   
/// Application Logging									   
#define RZX_TRACE(...)			::Razix::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RZX_INFO(...)			::Razix::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RZX_WARN(...)			::Razix::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RZX_ERROR(...)			::Razix::Log::GetCoreLogger()->error(__VA_ARGS__)
														   
//#endif