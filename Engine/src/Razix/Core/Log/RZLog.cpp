// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZLog.h"

#include "Razix/Core/std/sprintf.h"    // for rz_snprintf

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>

#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifndef __APPLE__    // disabled until I find a good solution cause apple is a bitch about creating files and directories without proper permissions
    #define ENABLE_FILE_LOGGING 1
#endif

#define LOG_MSG_PATTERN "%^ %n [%T] :: %v %$"

namespace Razix {
    namespace Debug {

        std::shared_ptr<spdlog::logger> RZLog::s_CoreLogger;
        std::shared_ptr<spdlog::logger> RZLog::s_ApplicationLogger;
        std::vector<spdlog::sink_ptr>   RZLog::s_CoreLoggerSinks;
        std::vector<spdlog::sink_ptr>   RZLog::s_AppLoggerSinks;

        static std::string GetTimestampedLogFileName()
        {
            auto        now   = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            std::tm     tm{};
#ifdef _WIN32
            localtime_s(&tm, &now_c);
#else
            localtime_r(&now_c, &tm);
#endif
            char logFileName[256];
            rz_snprintf(logFileName, sizeof(logFileName), "Logs/Razix_Engine_Log_%04d-%02d-%02d_%02d-%02d-%02d.log", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

            return std::string(logFileName);
        }

        void RZLog::StartUp()
        {
#if !defined(RAZIX_GOLD_MASTER) && defined(ENABLE_FILE_LOGGING)
    #if defined(__APPLE__) || defined(__UNIX__)
            int res = mkdir("Logs", 0755);
            RAZIX_ASSERT(!res, "Failed to crate log directory, check your permissions!");
    #else
            std::filesystem::create_directories("Logs");
    #endif

#endif

            // ----------------- CORE LOGGER SETUP -----------------
            s_CoreLoggerSinks.clear();
            auto core_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            core_console_sink->set_level(spdlog::level::trace);
#ifndef RAZIX_TESTS
            core_console_sink->set_pattern(LOG_MSG_PATTERN);
#endif
            s_CoreLoggerSinks.push_back(core_console_sink);

#ifdef ENABLE_FILE_LOGGING
            auto core_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(GetTimestampedLogFileName().c_str(), true);
            core_file_sink->set_level(spdlog::level::trace);
    #ifndef RAZIX_TESTS
            core_file_sink->set_pattern(LOG_MSG_PATTERN);
    #endif
            s_CoreLoggerSinks.push_back(core_file_sink);
#endif

            char coreLoggerName[64];
            rz_snprintf(coreLoggerName, sizeof(coreLoggerName), "%-18s", "Razix Core");
            s_CoreLogger = std::make_shared<spdlog::logger>(coreLoggerName, s_CoreLoggerSinks.begin(), s_CoreLoggerSinks.end());
            s_CoreLogger->set_level(spdlog::level::trace);
#ifdef ENABLE_FILE_LOGGING
            s_CoreLogger->flush_on(spdlog::level::trace);
#endif
            spdlog::register_logger(s_CoreLogger);

            RAZIX_CORE_INFO("Starting Up Core Engine Logger");

            // ----------------- APP LOGGER SETUP -----------------
            s_AppLoggerSinks.clear();
            auto app_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            app_console_sink->set_level(spdlog::level::trace);
#ifndef RAZIX_TESTS
            app_console_sink->set_pattern(LOG_MSG_PATTERN);
#endif
            s_AppLoggerSinks.push_back(app_console_sink);

#ifdef ENABLE_FILE_LOGGING
            auto app_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(GetTimestampedLogFileName().c_str(), true);
            app_file_sink->set_level(spdlog::level::trace);
    #ifndef RAZIX_TESTS
            app_file_sink->set_pattern(LOG_MSG_PATTERN);
    #endif
            s_AppLoggerSinks.push_back(app_file_sink);
#endif

            char appLoggerName[64];
            rz_snprintf(appLoggerName, sizeof(appLoggerName), "%-18s", "Razix App");
            s_ApplicationLogger = std::make_shared<spdlog::logger>(appLoggerName, s_AppLoggerSinks.begin(), s_AppLoggerSinks.end());
            s_ApplicationLogger->set_level(spdlog::level::trace);
#ifdef ENABLE_FILE_LOGGING
            s_ApplicationLogger->flush_on(spdlog::level::trace);
#endif
            spdlog::register_logger(s_ApplicationLogger);

            RAZIX_INFO("Starting Up Engine Application Logger");
        }

        void RZLog::Shutdown()
        {
            if (s_CoreLogger) {
                RAZIX_CORE_INFO("Shutting down Logging System");
                s_CoreLogger.reset();
            }
            if (s_ApplicationLogger)
                s_ApplicationLogger.reset();

            spdlog::shutdown();
        }

        void RZLog::RegisterCoreLoggerSink(spdlog::sink_ptr sink)
        {
            if (sink == nullptr)
                return;

            s_CoreLoggerSinks.push_back(sink);
            auto& core_sinks = s_CoreLogger->sinks();
            core_sinks.insert(core_sinks.end(), s_CoreLoggerSinks.begin(), s_CoreLoggerSinks.end());
        }

        void RZLog::RegisterApplicationLoggerSink(spdlog::sink_ptr sink)
        {
            if (sink == nullptr)
                return;

            s_AppLoggerSinks.push_back(sink);
            auto& app_sinks = s_ApplicationLogger->sinks();
            app_sinks.insert(app_sinks.end(), s_AppLoggerSinks.begin(), s_AppLoggerSinks.end());
        }
    }    // namespace Debug
}    // namespace Razix

void RZLog_StartUp(void)
{
    Razix::Debug::RZLog::StartUp();
}

void RZLog_Shutdown(void)
{
    Razix::Debug::RZLog::Shutdown();
}

void RZLog_CoreTrace(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Razix::Debug::RZLog::GetCoreLogger()->trace(args);
    va_end(args);
}

void RZLog_CoreInfo(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Razix::Debug::RZLog::GetCoreLogger()->info(args);
    va_end(args);
}

void RZLog_CoreWarn(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Razix::Debug::RZLog::GetCoreLogger()->warn(args);
    va_end(args);
}

void RZLog_CoreError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Razix::Debug::RZLog::GetCoreLogger()->error(args);
    va_end(args);
}

void RZLog_AppTrace(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Razix::Debug::RZLog::GetApplicationLogger()->trace(args);
    va_end(args);
}

void RZLog_AppInfo(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Razix::Debug::RZLog::GetApplicationLogger()->info(args);
    va_end(args);
}

void RZLog_AppWarn(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Razix::Debug::RZLog::GetApplicationLogger()->warn(args);
    va_end(args);
}

void RZLog_AppError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Razix::Debug::RZLog::GetApplicationLogger()->error(args);
    va_end(args);
}
