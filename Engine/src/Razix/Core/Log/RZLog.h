#pragma once

#ifdef __cplusplus

    #include "Razix/Core/RZCore.h"
    #include <spdlog/logger.h>

namespace Razix {
    namespace Debug {

        // TODO: Add categorization for sinks

        /** 
         * The Engine wide Logging system
         * @birief Used for various Engine and Client sub-systems to report statuses, errors and other information
         */
        class RAZIX_API RZLog
        {
        public:
            /* Starts Up the Logging system */
            static void StartUp();
            /* Shuts down the Logging system */
            static void Shutdown();

            /* Returns the shared pointer to the engine core logger */
            RAZIX_INLINE static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
            /* Returns the shared pointer to the engine application logger */
            RAZIX_INLINE static std::shared_ptr<spdlog::logger>& GetApplicationLogger() { return s_ApplicationLogger; }

            static void RegisterCoreLoggerSink(spdlog::sink_ptr sink);
            static void RegisterApplicationLoggerSink(spdlog::sink_ptr sink);

        private:
            static std::shared_ptr<spdlog::logger> s_CoreLogger;
            static std::shared_ptr<spdlog::logger> s_ApplicationLogger;
            static std::vector<spdlog::sink_ptr>   s_CoreLoggerSinks;
            static std::vector<spdlog::sink_ptr>   s_AppLoggerSinks;
        };
    }    // namespace Debug
}    // namespace Razix

#endif    // __cplusplus

#ifdef __cplusplus
extern "C"
{
#endif

    void RZLog_CoreLog_Trace(const char* fmt, ...);
    void RZLog_CoreLog_Info(const char* fmt, ...);
    void RZLog_CoreLog_Warn(const char* fmt, ...);
    void RZLog_CoreLog_Error(const char* fmt, ...);
    void RZLog_AppLog_Trace(const char* fmt, ...);
    void RZLog_AppLog_Info(const char* fmt, ...);
    void RZLog_AppLog_Warn(const char* fmt, ...);
    void RZLog_AppLog_Error(const char* fmt, ...);

#ifdef __cplusplus
}
#endif

// Strip out the logging system in the distribution build
#ifndef RAZIX_GOLD_MASTER

    // Core Engine Logging
    #define RAZIX_CORE_TRACE(...) RZLog_CoreLog_Trace(__VA_ARGS__)
    #define RAZIX_CORE_INFO(...)  RZLog_CoreLog_Info(__VA_ARGS__)
    #define RAZIX_CORE_WARN(...)  RZLog_CoreLog_Warn(__VA_ARGS__)
    #define RAZIX_CORE_ERROR(...) RZLog_CoreLog_Error(__VA_ARGS__)

    // Application Logging
    #define RAZIX_TRACE(...) RZLog_AppLog_Trace(__VA_ARGS__)
    #define RAZIX_INFO(...)  RZLog_AppLog_Info(__VA_ARGS__)
    #define RAZIX_WARN(...)  RZLog_AppLog_Warn(__VA_ARGS__)
    #define RAZIX_ERROR(...) RZLog_AppLog_Error(__VA_ARGS__)
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
