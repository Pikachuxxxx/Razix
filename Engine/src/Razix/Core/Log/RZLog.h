#ifndef _RZ_LOG_H_
#define _RZ_LOG_H_

#include "Razix/Core/RZCore.h"

#ifdef __cplusplus
    #include <spdlog/spdlog.h>

namespace Razix {
    namespace Debug {

        //============================================================================
        // !!! EXCEPTION !!!
        // This file is ALLOWED to use std::vector and STL containers
        //
        // REASON: RZLog is a foundational system used to log errors in other
        // containers and is the FIRST system initialized in the engine.
        // Using Razix custom containers here would create CYCLIC DEPENDENCIES.
        //
        // This exception is EXCLUDED from GoldMaster builds anyway (see macros below),
        // so no STL overhead in production builds.
        //============================================================================

        // TODO: Add categorization for sinks

        /** 
         * The Engine wide Logging system
         * @birief Used for various Engine and Client sub-systems to report statuses, errors and other information
         */
        class RAZIX_API RZLog
        {
        public:
            static void StartUp();
            static void Shutdown();

            RAZIX_INLINE static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
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
#endif    // __cplusplus

    // Initialization wrappers
    RAZIX_API void RZLog_StartUp(void);
    RAZIX_API void RZLog_Shutdown(void);

    // Logging wrappers (Printf style for C compatibility)
    RAZIX_API void RZLog_CoreTrace(const char* fmt, ...);
    RAZIX_API void RZLog_CoreInfo(const char* fmt, ...);
    RAZIX_API void RZLog_CoreWarn(const char* fmt, ...);
    RAZIX_API void RZLog_CoreError(const char* fmt, ...);

    RAZIX_API void RZLog_AppTrace(const char* fmt, ...);
    RAZIX_API void RZLog_AppInfo(const char* fmt, ...);
    RAZIX_API void RZLog_AppWarn(const char* fmt, ...);
    RAZIX_API void RZLog_AppError(const char* fmt, ...);

#ifdef __cplusplus
}
#endif    // __cplusplus

#ifndef RAZIX_GOLD_MASTER

    #ifdef __cplusplus
        #define RAZIX_CORE_TRACE(...) ::Razix::Debug::RZLog::GetCoreLogger()->trace(__VA_ARGS__)
        #define RAZIX_CORE_INFO(...)  ::Razix::Debug::RZLog::GetCoreLogger()->info(__VA_ARGS__)
        #define RAZIX_CORE_WARN(...)  ::Razix::Debug::RZLog::GetCoreLogger()->warn(__VA_ARGS__)
        #define RAZIX_CORE_ERROR(...) ::Razix::Debug::RZLog::GetCoreLogger()->error(__VA_ARGS__)

        #define RAZIX_TRACE(...) ::Razix::Debug::RZLog::GetApplicationLogger()->trace(__VA_ARGS__)
        #define RAZIX_INFO(...)  ::Razix::Debug::RZLog::GetApplicationLogger()->info(__VA_ARGS__)
        #define RAZIX_WARN(...)  ::Razix::Debug::RZLog::GetApplicationLogger()->warn(__VA_ARGS__)
        #define RAZIX_ERROR(...) ::Razix::Debug::RZLog::GetApplicationLogger()->error(__VA_ARGS__)
    #else
        #define RAZIX_CORE_TRACE(...) RZLog_CoreTrace(__VA_ARGS__)
        #define RAZIX_CORE_INFO(...)  RZLog_CoreInfo(__VA_ARGS__)
        #define RAZIX_CORE_WARN(...)  RZLog_CoreWarn(__VA_ARGS__)
        #define RAZIX_CORE_ERROR(...) RZLog_CoreError(__VA_ARGS__)

        #define RAZIX_TRACE(...) RZLog_AppTrace(__VA_ARGS__)
        #define RAZIX_INFO(...)  RZLog_AppInfo(__VA_ARGS__)
        #define RAZIX_WARN(...)  RZLog_AppWarn(__VA_ARGS__)
        #define RAZIX_ERROR(...) RZLog_AppError(__VA_ARGS__)
    #endif    // __cplusplus
#else
    // Strip everything in Gold Master
    #define RAZIX_CORE_TRACE(...)
    #define RAZIX_CORE_INFO(...)
    #define RAZIX_CORE_WARN(...)
    #define RAZIX_CORE_ERROR(...)
    #define RAZIX_TRACE(...)
    #define RAZIX_INFO(...)
    #define RAZIX_WARN(...)
    #define RAZIX_ERROR(...)
#endif    // RAZIX_GOLD_MASTER

#endif    // _RZ_LOG_H_
