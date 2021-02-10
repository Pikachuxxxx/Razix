#pragma once

#include "Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

namespace Razix
{
    /// The Engine wide Logging system
    ///
    /// Used for various Engine and Client sub-systems to report statuses, errors and other information
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

// Strip out the logging system in the distribution build
#ifndef RZX_DIST

// Core Engine Logging 
#define RZX_CORE_TRACE(...)		::Razix::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RZX_CORE_INFO(...)		::Razix::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RZX_CORE_WARN(...)		::Razix::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RZX_CORE_ERROR(...)		::Razix::Log::GetCoreLogger()->error(__VA_ARGS__)
                                                           
// Application Logging									   
#define RZX_TRACE(...)			::Razix::Log::GetApplicationLogger()->trace(__VA_ARGS__)
#define RZX_INFO(...)			::Razix::Log::GetApplicationLogger()->info(__VA_ARGS__)
#define RZX_WARN(...)			::Razix::Log::GetApplicationLogger()->warn(__VA_ARGS__)
#define RZX_ERROR(...)			::Razix::Log::GetApplicationLogger()->error(__VA_ARGS__)
                                                           
#else
// Core Engine Logging 
#define RZX_CORE_TRACE(...)
#define RZX_CORE_INFO(...)
#define RZX_CORE_WARN(...)
#define RZX_CORE_ERROR(...)

// Application Logging	
#define RZX_TRACE(...)	
#define RZX_INFO(...)	
#define RZX_WARN(...)	
#define RZX_ERROR(...)	
#endif