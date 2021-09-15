#include "rzxpch.h"
#include "Log.h"

#include "Razix/Core/SplashScreen.h"

namespace Razix
{
    namespace Debug {

        std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
        std::shared_ptr<spdlog::logger> Log::s_ApplicationLogger;

        void Log::StartUp()
        {
            // TODO: Add src file and line logging to the pattern
            // Try using __FUNCTION__, __LINE__ and __FILE__ etc.instead of using Macro-ed global SPDLog functions
            spdlog::set_pattern("%^ %n [%T] :: %v %$");

            std::stringstream coreLoggerName;
			coreLoggerName << std::setw(18) << std::left << "Razix Core";

            s_CoreLogger = spdlog::stdout_color_mt(coreLoggerName.str());
            s_CoreLogger->set_level(spdlog::level::trace);
            RAZIX_CORE_INFO("Starting Up Core Engine Logger");
            Razix::SplashScreen::Get().SetLogString("Starting Up Core Engine Logger");

			std::stringstream appLoggerName;
			appLoggerName << std::setw(18) << std::left << "Razix Application";

            s_ApplicationLogger = spdlog::stdout_color_mt(appLoggerName.str());
            s_ApplicationLogger->set_level(spdlog::level::trace);
            RAZIX_INFO("Starting Up Engine Application Logger");
            Razix::SplashScreen::Get().SetLogString("Starting Up Engine Application Logger");
        }

        void Log::Shutdown()
        {
            RAZIX_CORE_ERROR("Shutting down Logging System");
            s_CoreLogger.reset();
            s_ApplicationLogger.reset();
            spdlog::shutdown();
        }

    }
}