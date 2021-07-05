#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/Reference.h"

#pragma warning(push, 0)
#pragma warning( disable : 26812)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Razix {
    namespace Debug
    {
        /// The Engine wide Logging system
        ///
        /// Used for various Engine and Client sub-systems to report statuses, errors and other information
        class RAZIX_API Log
        {
        public:
            static void StartUp();
            static void Shutdown();

            inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
            inline static Ref<spdlog::logger>& GetApplicationLogger() { return s_ApplicationLogger; }
        private:
			static Ref<spdlog::logger> s_CoreLogger;
			static Ref<spdlog::logger> s_ApplicationLogger;
        };
    }
}

// Strip out the logging system in the distribution build
#ifndef RAZIX_DISTRIBUTION

// Core Engine Logging 
#define RAZIX_CORE_TRACE(...)		::Razix::Debug::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RAZIX_CORE_INFO(...)		::Razix::Debug::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RAZIX_CORE_WARN(...)		::Razix::Debug::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RAZIX_CORE_ERROR(...)		::Razix::Debug::Log::GetCoreLogger()->error(__VA_ARGS__)
                                                           
// Application Logging									   
#define RAZIX_TRACE(...)			::Razix::Debug::Log::GetApplicationLogger()->trace(__VA_ARGS__)
#define RAZIX_INFO(...)			    ::Razix::Debug::Log::GetApplicationLogger()->info(__VA_ARGS__)
#define RAZIX_WARN(...)			    ::Razix::Debug::Log::GetApplicationLogger()->warn(__VA_ARGS__)
#define RAZIX_ERROR(...)			::Razix::Debug::Log::GetApplicationLogger()->error(__VA_ARGS__)
                                                           
#else
// Core Engine Logging 
#define RAZIX_CORE_TRACE(...)
#define RAZIX_CORE_INFO(...)
#define RAZIX_CORE_WARN(...)
#define RAZIX_CORE_ERROR(...)

// Application Logging	
#define RAZIX_TRACE(...)	
#define RAZIX_INFO(...)	
#define RAZIX_WARN(...)	
#define RAZIX_ERROR(...)	
#endif