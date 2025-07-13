// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZLog.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>

#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

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
            std::ostringstream oss;
            oss << "Logs/Razix_Engine_Log_" << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << ".log";
            return oss.str();
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
            auto core_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(GetTimestampedLogFileName(), true);
            core_file_sink->set_level(spdlog::level::trace);
    #ifndef RAZIX_TESTS
            core_file_sink->set_pattern(LOG_MSG_PATTERN);
    #endif
            s_CoreLoggerSinks.push_back(core_file_sink);
#endif

            std::stringstream coreLoggerName;
            coreLoggerName << std::setw(18) << std::left << "Razix Core";
            s_CoreLogger = std::make_shared<spdlog::logger>(coreLoggerName.str(), s_CoreLoggerSinks.begin(), s_CoreLoggerSinks.end());
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
            auto app_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(GetTimestampedLogFileName(), true);
            app_file_sink->set_level(spdlog::level::trace);
    #ifndef RAZIX_TESTS
            app_file_sink->set_pattern(LOG_MSG_PATTERN);
    #endif
            s_AppLoggerSinks.push_back(app_file_sink);
#endif

            std::stringstream appLoggerName;
            appLoggerName << std::setw(18) << std::left << "Razix App";
            s_ApplicationLogger = std::make_shared<spdlog::logger>(appLoggerName.str(), s_AppLoggerSinks.begin(), s_AppLoggerSinks.end());
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

#define LOG_BUFFER_SIZE 2048

static void FormatLogAndSend(const std::shared_ptr<spdlog::logger>& logger,
    spdlog::level::level_enum                                       level,
    const char*                                                     fmt,
    va_list                                                         args)
{
    char buffer[LOG_BUFFER_SIZE];
    vsnprintf(buffer, LOG_BUFFER_SIZE, fmt, args);

    switch (level) {
        case spdlog::level::trace: logger->trace(buffer); break;
        case spdlog::level::info: logger->info(buffer); break;
        case spdlog::level::warn: logger->warn(buffer); break;
        case spdlog::level::err: logger->error(buffer); break;
        default: break;
    }
}

extern "C"
{
    void RZLog_CoreLog_Trace(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        FormatLogAndSend(Razix::Debug::RZLog::GetCoreLogger(), spdlog::level::trace, fmt, args);
        va_end(args);
    }

    void RZLog_CoreLog_Info(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        FormatLogAndSend(Razix::Debug::RZLog::GetCoreLogger(), spdlog::level::info, fmt, args);
        va_end(args);
    }

    void RZLog_CoreLog_Warn(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        FormatLogAndSend(Razix::Debug::RZLog::GetCoreLogger(), spdlog::level::warn, fmt, args);
        va_end(args);
    }

    void RZLog_CoreLog_Error(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        FormatLogAndSend(Razix::Debug::RZLog::GetCoreLogger(), spdlog::level::err, fmt, args);
        va_end(args);
    }

    void RZLog_AppLog_Trace(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        FormatLogAndSend(Razix::Debug::RZLog::GetApplicationLogger(), spdlog::level::trace, fmt, args);
        va_end(args);
    }

    void RZLog_AppLog_Info(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        FormatLogAndSend(Razix::Debug::RZLog::GetApplicationLogger(), spdlog::level::info, fmt, args);
        va_end(args);
    }

    void RZLog_AppLog_Warn(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        FormatLogAndSend(Razix::Debug::RZLog::GetApplicationLogger(), spdlog::level::warn, fmt, args);
        va_end(args);
    }

    void RZLog_AppLog_Error(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        FormatLogAndSend(Razix::Debug::RZLog::GetApplicationLogger(), spdlog::level::err, fmt, args);
        va_end(args);
    }
}