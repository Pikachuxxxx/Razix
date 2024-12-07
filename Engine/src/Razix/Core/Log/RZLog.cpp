// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZLog.h"

//RAZIX_WARNING_PUSH()
//#pragma warning(disable : 26812)
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
//RAZIX_WARNING_POP()

namespace Razix {
    namespace Debug {

        std::shared_ptr<spdlog::logger> RZLog::s_CoreLogger;
        std::shared_ptr<spdlog::logger> RZLog::s_ApplicationLogger;
        std::vector<spdlog::sink_ptr>   RZLog::s_CoreLoggerSinks;
        std::vector<spdlog::sink_ptr>   RZLog::s_AppLoggerSinks;

        void RZLog::StartUp()
        {
#ifndef RAZIX_TESTS
            // Set the pattern for log message
            spdlog::set_pattern("%^ %n [%T] :: %v %$");
#endif

            std::stringstream coreLoggerName;
            coreLoggerName << std::setw(18) << std::left << "Razix Core";

            s_CoreLogger = spdlog::stdout_color_mt(coreLoggerName.str());
            s_CoreLogger->set_level(spdlog::level::trace);
            // Add the sinks
            auto& core_sinks = s_CoreLogger->sinks();
            core_sinks.insert(core_sinks.end(), s_CoreLoggerSinks.begin(), s_CoreLoggerSinks.end());

            RAZIX_CORE_INFO("Starting Up Core Engine Logger");

            //--------------------------------------------------------------------------------------

            std::stringstream appLoggerName;
            appLoggerName << std::setw(18) << std::left << "Razix App";

            s_ApplicationLogger = spdlog::stdout_color_mt(appLoggerName.str());
            // Add the sinks
            auto& app_sinks = s_ApplicationLogger->sinks();
            app_sinks.insert(app_sinks.end(), s_AppLoggerSinks.begin(), s_AppLoggerSinks.end());
            s_ApplicationLogger->set_level(spdlog::level::trace);
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
