// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZLog.h"

#include "Razix/Core/RZSplashScreen.h"

#include "internal/RazixMemory/include/RZMemoryLog.h"

namespace Razix {
    namespace Debug {

        std::shared_ptr<spdlog::logger> RZLog::s_CoreLogger;
        std::shared_ptr<spdlog::logger> RZLog::s_ApplicationLogger;

        void RZLog::StartUp()
        {
            // Set the pattern for log message
            spdlog::set_pattern("%^ %n [%T] :: %v %$");

            std::stringstream coreLoggerName;
            coreLoggerName << std::setw(18) << std::left << "Razix Core";

            s_CoreLogger = spdlog::stdout_color_mt(coreLoggerName.str());
            s_CoreLogger->set_level(spdlog::level::trace);
            RAZIX_CORE_INFO("Starting Up Core Engine Logger");
            Razix::RZSplashScreen::Get().setLogString("Starting Up Core Engine Logger");

            //--------------------------------------------------------------------------------------

            std::stringstream appLoggerName;
            appLoggerName << std::setw(18) << std::left << "Razix Application";

            s_ApplicationLogger = spdlog::stdout_color_mt(appLoggerName.str());
            s_ApplicationLogger->set_level(spdlog::level::trace);
            RAZIX_INFO("Starting Up Engine Application Logger");
            Razix::RZSplashScreen::Get().setLogString("Starting Up Engine Application Logger");

            // Start the memory logger
            RZMemoryLog::StartUp();
        }

        void RZLog::Shutdown()
        {
            RAZIX_CORE_ERROR("Shutting down Logging System");
            s_CoreLogger.reset();
            s_ApplicationLogger.reset();

            RZMemoryLog::Shutdown();

            spdlog::shutdown();
        }

    }    // namespace Debug
}    // namespace Razix